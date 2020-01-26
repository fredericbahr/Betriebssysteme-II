#include <assert.h>
#include "project.h"
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

void show_help(char *name)
{
	fprintf(stderr,"Usage HINTS for %s\n", name);
  fprintf(stderr,"%s -t 24473 -u 24473 -b -m 24473 -p ip.txt\n", name);
  fprintf(stderr,"-t <ushort>: portnumber for tcp socket \n");
  fprintf(stderr,"-u <ushort>: portnumber for udp socket \n");
  fprintf(stderr,"-b : start as daemon - not implemented\n");
  fprintf(stderr,"-p : path for the file, where LSP are saved\n");
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

	while ((c = getopt(argc, argv, "ht:u:bp:")) != -1)
		switch (c) {
		case 't':
			{
				assert(optarg);
				tmp = atoll(optarg);
				if (tmp > 5000)
					handle.web_portnummer =
					    (unsigned short int)tmp % 65535;
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
					syslog_x(LOG_INFO, "Die udp_portnumber wurde auf %d geändert.\n", handle.udp_portnummer);
				}
				else {
					syslog_x(LOG_CRIT,
						 "udp_portnumber remains %d (%d not allowed)\n",
						 handle.udp_portnummer, tmp);
				}
				break;
			}
    case 'b': {
      ///todo set flag - daemon shall NOT run in CONSOLE but in BACKGROUND
      break;
    }
	case 'p': {
      handle.path = optarg;
	  syslog_x(LOG_INFO, "Filepath wurde auf %s geändert\n", handle.path);
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
	serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);	//htonl (INADDR_ANY);
	serverinfo.sin_port = htons(handle.web_portnummer);
	setsockopt(handle.web_server_socket, SOL_SOCKET, SO_REUSEADDR, &flag,
		   sizeof(flag));

	laenge = sizeof(serverinfo);

	retval =
	    bind(handle.web_server_socket, (struct sockaddr *)&serverinfo,
		 laenge);
	if (retval == -1) {
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

void beende(int sig) {
	exit(EXIT_FAILURE);
}

int udp_init_socket() {
	int laenge;
	socklen_t len;
	int retval = 0;
	int n;
	int flag = 1;
	int anzIP = 0;
	struct sockaddr_in serverinfo, clientinfo;
	char puffer[BUFFERSIZE];
	char bestaetigungsNachricht[] = "SERVER: Habe die Daten erhalten.\n";
	
	signal(SIGINT, beende);

	//socket erzeugen
	handle.udp_peer_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (handle.udp_peer_socket < 0) {
		printf ("Kann Server-Socket nicht öffnen ...\n");
		exit (EXIT_FAILURE);
	}

	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);	// all network interfaces
	serverinfo.sin_port = htons(handle.udp_portnummer); //inital handle.udp_portnummer
	
	setsockopt(handle.udp_peer_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	laenge = sizeof(serverinfo);

	retval = bind(handle.udp_peer_socket, (struct sockaddr *)&serverinfo, laenge);
		 
	if (retval == -1) {
		syslog_x(LOG_CRIT, "UDP PEER Socket INIT failed on bind. ");
		close(handle.udp_peer_socket);
		handle.udp_peer_socket = -1;
	}

	syslog_x(LOG_INFO, "Warte auf Konfiguration durch den Client.\n"); 

	FD_SET(handle.udp_peer_socket, &handle.rfds);	// register socket for select
	
	if (handle.max_socket < handle.udp_peer_socket) {
		handle.max_socket = handle.udp_peer_socket;	// need max value in handle.rfds for select
	}
	
	/* Abfrage wie viele Argumente es gibt */
	
	/* Puffer initialisieren */
	memset (puffer, 0, BUFFERSIZE);
	
	/* Nachrichten empfangen */
	len = sizeof (clientinfo);
	
	n = recvfrom ( handle.udp_peer_socket, puffer, BUFFERSIZE, 0, (struct sockaddr *) &clientinfo, &len );
	if (n < 0) {
	   printf ("Kann keine Daten empfangen ...\n");
	}

	/* Erhaltene Nachricht ausgeben */
	//Speichere IP-Addressen in struct
	syslog_x(LOG_INFO, "Anzahl von IP-Adressen erhalten von %s an Port %u : %s \n", inet_ntoa (clientinfo.sin_addr),
			ntohs (clientinfo.sin_port), puffer);
	
	anzIP = atoi(puffer);
	
	n = sendto (handle.udp_peer_socket, bestaetigungsNachricht, strlen(bestaetigungsNachricht), 0, (struct sockaddr *) &clientinfo, sizeof (clientinfo));
	if(n < 0) {
		syslog_x(LOG_CRIT, "Konnte keine Bestätigungsnachricht schicken.\n");
	}
	

	FILE *fp;
	fp = fopen(handle.path, "w");

	getOwnIP_Port(fp);
	
	/* weitere Nachrichten Abfangen */
	while (anzIP--) {
		/* Puffer initialisieren */
		memset (puffer, 0, BUFFERSIZE);
		/* Nachrichten empfangen */
		len = sizeof (clientinfo);
		n = recvfrom ( handle.udp_peer_socket, puffer, BUFFERSIZE, 0, (struct sockaddr *) &clientinfo, &len );
		if (n < 0) {
		   printf ("Kann keine Daten empfangen ...\n");
		   continue;
		}

		/* Erhaltene Nachricht verarbeiten */
		
		syslog_x(LOG_INFO, "Daten erhalten von %s an Port %u : %s \n", inet_ntoa (clientinfo.sin_addr),
				ntohs (clientinfo.sin_port), puffer);
		
		/* IP-Adresse in Datei speichern */ 
		n = fprintf(fp, puffer);
		if(n < 0) {
			syslog_x(LOG_ALERT, "Fehler beim schreiben in die Datei!\n Beende Programm!");
			exit(EXIT_FAILURE);
		}
		n = fprintf(fp, "\n");
		if(n < 0) {
			syslog_x(LOG_ALERT, "Fehler beim schreiben in die Datei!\n Beende Programm!");
			exit(EXIT_FAILURE);
		}
		/* sende Bestätigung an CLIENT */
		n = sendto (handle.udp_peer_socket, bestaetigungsNachricht, strlen(bestaetigungsNachricht), 0, (struct sockaddr *) &clientinfo, sizeof (clientinfo));
		if(n < 0) {
			syslog_x(LOG_CRIT, "Konnte keine Bestätigungsnachricht schicken.\n");
		}
	}
	fclose(fp);
	return retval;
}

void getOwnIP_Port(FILE * fp) {

    char hostbuffer[256];
	char * IPBuffer = malloc(11* sizeof(char));
	char *ownport;
	int hostname;
	struct hostent *host_entry;

	hostname = gethostname(hostbuffer, sizeof (hostbuffer));
	if(hostname == -1) {
		syslog_x(LOG_ALERT, "Konnte die eigene Ip-Adresse nicht ermitteln");
		perror("gethostname");
	}
	host_entry = gethostbyname(hostbuffer);
	if(host_entry == NULL) {
		syslog_x(LOG_ALERT, "Konnte den Host nicht am Namen identifizieren");
		perror("gethostname");
	}
	
	IPBuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    
	sprintf(ownport, "%d", handle.udp_portnummer);

	fprintf(fp, IPBuffer);
	fprintf(fp, ":");
	fprintf(fp, ownport);
	fprintf(fp, "\n");

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

	handle.path = "ip.txt";

	parse_arguments(argc, argv); // <-- eventualy override port numbers etc.
	
	/* register cleanup function */
	atexit(cleanup);

	/* register signals */
	my_signal(SIGTERM, catch_sigterm);
	my_signal(SIGINT, catch_sigint);
	my_signal(SIGALRM, catch_sigalarm);

	/* create an TCP-Socket @ localhost */
	/*
	if (0 != web_init_socket()) {
		return -1;
	}			// end if failed
	*/

	if (0 != udp_init_socket()) {
		return -1;
	}			// end if failed

	return 0;
}
