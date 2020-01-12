#include <assert.h>
#include "project.h"

void show_help(char *name)
{
	fprintf(stderr,"Usage HINTS for %s\n", name);
  fprintf(stderr,"%s -t 24473 -u 24473 -b -m 24473\n", name);
  fprintf(stderr,"-t <ushort>: portnumber for tcp socket \n");
  fprintf(stderr,"-u <ushort>: portnumber for udp socket \n");
  fprintf(stderr,"-b : start as daemon - not implemented\n");
  fprintf(stderr,"-m <ushort>: secret for msg/unix domain socket queue - not implemented \n");
  fprintf(stderr,"\n to test:\n open second console and execute\n");
  fprintf(stderr,"\t nc 127.0.0.1 24473 # to connect to tcp socket\n");
  fprintf(stderr,"\t nc -u 127.0.0.1 24473 # to connect to udp socket\n");
	exit(0);
}

void parse_arguments(int argc, char **argv)
{
	char c;
	long tmp;

	while ((c = getopt(argc, argv, "ht:u:bm:")) != -1)
		switch (c) {
		case 't':
			{
				assert(optarg);
				tmp = atoll(optarg);
				if (tmp > 5000) {
					handle.web_portnummer =
					    (unsigned short int)tmp % 65535;
					syslog_x(LOG_INFO, "New Wep Portnumber: %d\n", handle.web_portnummer);
				}
				else
					syslog_x(LOG_CRIT,
						 "web_portnumber remains %d (%d not allowed)\n",
						 handle.web_portnummer, tmp);
				break;
			}
		case 'u':
			{
				assert(optarg);
				tmp = atoll(optarg);
				if (tmp > 5000) {
					handle.udp_portnummer =
					    (unsigned short int)tmp % 65535;
					syslog_x(LOG_INFO, "New UPD Portnumber: %d\n", handle.udp_portnummer);
				}
				else
					syslog_x(LOG_CRIT,
						 "udp_portnumber remains %d (%d not allowed)\n",
						 handle.udp_portnummer, tmp);
				break;
			}
    case 'b': {
      ///todo set flag - daemon shall NOT run in CONSOLE but in BACKGROUND
      break;
    }

	case 'm': {
			assert(optarg);
			handle.secret = atoll(optarg);
			syslog_x(LOG_INFO, "Secret key is: %d\n", handle.secret);
			break;
	}
		case 'h':
		default:
			show_help(argv[0]);
		}
}

int web_init_socket()
{
	int laenge;
	struct sockaddr_in serverinfo;
	int retval = 0;
	int flag = 1;

	handle.web_server_socket = socket(PF_INET, SOCK_STREAM, 0);

	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = inet_addr("127.0.0.1");	//htonl (INADDR_ANY);
	serverinfo.sin_port = htons(handle.web_portnummer);
	setsockopt(handle.web_server_socket, SOL_SOCKET, SO_REUSEADDR, &flag,
		   sizeof(flag));

	laenge = sizeof(serverinfo);

	retval =
	    bind(handle.web_server_socket, (struct sockaddr *)&serverinfo,
		 laenge);
	if (-1 == retval) {
		syslog_x(LOG_CRIT, "Web Server Socket INIT failed on bind. ");
    close(handle.web_server_socket);
		handle.web_server_socket = -1;
	}

	listen(handle.web_server_socket, 2);
	FD_SET(handle.web_server_socket, &handle.rfds);	// register socket for select
	if (handle.max_socket < handle.web_server_socket)
		handle.max_socket = handle.web_server_socket;	// need max value in handle.rfds for select

	return retval;
}

int udp_init_socket()
{
	int laenge;
	struct sockaddr_in serverinfo;
	int retval = 0;
	int flag = 1;

	handle.udp_peer_socket = socket(PF_INET, SOCK_DGRAM, 0);

	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);	// all network interfaces
	serverinfo.sin_port = htons(handle.udp_portnummer);
	setsockopt(handle.udp_peer_socket, SOL_SOCKET, SO_REUSEADDR, &flag,
		   sizeof(flag));

	laenge = sizeof(serverinfo);

	retval =
	    bind(handle.udp_peer_socket, (struct sockaddr *)&serverinfo,
		 laenge);
	if (-1 == retval) {
		syslog_x(LOG_CRIT, "UDP PEER Socket INIT failed on bind. ");
    close(handle.udp_peer_socket);
		handle.udp_peer_socket = -1;
	}

	FD_SET(handle.udp_peer_socket, &handle.rfds);	// register socket for select
	if (handle.max_socket < handle.udp_peer_socket)
		handle.max_socket = handle.udp_peer_socket;	// need max value in handle.rfds for select

	return retval;
}

int init_everything(int argc, char **argv)
{
	bzero((void *)&handle, sizeof(struct handle_struct));	// set every value to zero
	// set no zero values to "not inited" == -1
	handle.web_server_socket = -1;
	handle.udp_peer_socket = -1;
  	// set std port numbers
  	handle.web_portnummer = 24473;
	handle.udp_portnummer = 24473;

	parse_arguments(argc, argv); // <-- eventualy override port numbers etc.
	
	/* register cleanup function */
	atexit(cleanup);

	/* register signals */
	my_signal(SIGTERM, catch_sigterm);
	my_signal(SIGINT, catch_sigint);
	my_signal(SIGALRM, catch_sigalarm);

	/* create an TCP-Socket @ localhost */
	if (0 != web_init_socket()) {
		return -1;
	}			// end if failed

	if (0 != udp_init_socket()) {
		return -1;
	}			// end if failed

	return 0;
}
