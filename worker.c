#include "project.h"

/*
 * test with nc 127.0.0.1 24473
 * */
int worker_new_web_request()
{
	int tmp;
	int retval = 0;
	socklen_t laenge;
	struct sockaddr_in clientinfo;
	char msg[] = "<pre>Hello</pre>";

	tmp =
	    accept(handle.web_server_socket, (struct sockaddr *)&clientinfo,
		   &laenge);
	// send last or STD message - no matter what the client requested
	if (0 != handle.lastmsg[0])
    retval = send(tmp, handle.lastmsg, sizeof(handle.lastmsg) - 1, 0);
  else
    retval = send(tmp, &msg, sizeof(msg) - 1, 0);
	if (1 > retval)
		syslog_x(LOG_CRIT, "less than One byte sent to web_client\n");
	// and close == http 0.9
	close(tmp);
	return 0;
}

/*
 * test with nc -u 127.0.0.1 24473
 * */
int worker_new_udp_request()
{
	ssize_t retval = 0;
	socklen_t len;
	struct sockaddr_in clientinfo;
	char buffer[BUFFERSIZE+1];

	memset (buffer, 0, BUFFERSIZE);

	len = sizeof (clientinfo);
	retval = recvfrom ( handle.udp_peer_socket, buffer, BUFFERSIZE, 0, (struct sockaddr *) &clientinfo, &len );
	if (1 > retval){
		syslog_x(LOG_CRIT, "less than One byte received by udp_peer\n");
		return -1;
	} else  {
		buffer[1+retval]=0;
		memcpy(handle.lastmsg,buffer,1+retval);
	}	
  
	// echo that
	syslog_x(LOG_INFO, buffer);
	retval = sendto (handle.udp_peer_socket, buffer, 1+ retval , 0, (struct sockaddr *) &clientinfo, sizeof (clientinfo)); 
  
  
	if (1 > retval){
		syslog_x(LOG_CRIT, "returning msg not successfull udp_peer\n");
		return -1;
	}
	return 0;
}
/*
 * 
 * */
int sende_lsp(char *dest)
{
	ssize_t retval = 0;
	socklen_t len;
	struct sockaddr_in clientinfo;
	char buffer[BUFFERSIZE+1];


	clientinfo.sin_family = AF_INET;
	clientinfo.sin_addr.s_addr = inet_addr(dest);	//htonl (INADDR_ANY);
	clientinfo.sin_port = htons(handle.udp_portnummer);

	// sende LSP an angegebene IP-Addresse
	retval = sendto (handle.udp_peer_socket, buffer, 1+ retval , 0, 
                    (struct sockaddr *) &clientinfo, sizeof (clientinfo));
	if (1 > retval) {
		syslog_x(LOG_CRIT, "returning msg not successfull udp_peer\n");
		return -1;
	} 
	return 0;
}

int worker()
{

	struct timeval tv;
	fd_set rfds;
	int retval = 0;

	syslog_x(LOG_INFO, "Bin nun im Worker"); 

	// select shall return once per second
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	// copy global bitvector into local copy
	memcpy(&rfds, &handle.rfds, sizeof(fd_set));
	//fprintf(stderr,"worker before select %d\n",retval);
	retval = select(handle.max_socket + 1, &rfds, NULL, NULL, &tv);
	//fprintf(stderr,"worker after select %d\n",retval);

	if (retval > 0) {	//fprintf(stderr, "ISSET == %d\n",FD_ISSET (handle.web_server_socket, &rfds));
		if (FD_ISSET(handle.web_server_socket, &rfds)) {
			retval = worker_new_web_request();
		} // end if web_server
		else if (FD_ISSET(handle.udp_peer_socket, &rfds)) {
			retval = worker_new_udp_request();
		} // end if udp_peer

    /* 
     * Implement MSG_QUEUE here to control DAEMON per
     * */

	}			// end if retval
//fprintf(stderr,"worker leaving %d\n",retval);
	return retval;
}
