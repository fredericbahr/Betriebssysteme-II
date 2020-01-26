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
int worker_new_udp_request() {
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

	return 0;
}
/*
 * 
 * */

void noNewNeighbors() {
	exit(EXIT_FAILURE);
}


int sende_lsp(char *dest, unsigned short int port, char *nachricht) {
	int retval = 0;
	struct sockaddr_in clientinfo;

	clientinfo.sin_family = AF_INET;
	clientinfo.sin_addr.s_addr = inet_addr(dest);	//htonl (INADDR_ANY);
	clientinfo.sin_port = htons(port);

	// sende LSP an angegebene IP-Addresse
	retval = sendto (handle.udp_peer_socket, nachricht, strlen(nachricht) , 0, 
                    (struct sockaddr *) &clientinfo, sizeof (clientinfo));
	if (retval < 1) {
		syslog_x(LOG_CRIT, "Konnte keine Nachricht an die jeweilige Adresse %s:%s senden\n", dest, port);
		return -1;
	} 
	return 0;
}

char * readWholeFile() {
	FILE *fp;
	long fsize;
	char * nachricht;

	fp = fopen(handle.path,"r");

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	nachricht = malloc(fsize + 1);
	fread(nachricht, 1, fsize, fp);

	return nachricht;
}

void sendInitialLSP() {
	FILE *fp;
	char ip[100];
	char port[100];
	int dontSendFirstLine = 1;
	char *nachricht;

	nachricht = readWholeFile();

	fp = fopen(handle.path, "r");

	while (fscanf(fp, "%[^:]%*c%[^\n]%*c", ip, port) != EOF)
	{
		if(!(dontSendFirstLine >= 1)) {
			sende_lsp(ip, atoi(port), nachricht);
		}
		else
		{
			dontSendFirstLine--;
		}
		
	}
	
}


void resendIncomingLSP(struct sockaddr_in neighborinfo) {
	FILE *fp;
	char ip[100];
	char port[100];
	int dontSendFirstLine = 1;
	char *nachricht;
	char neighborip[100];
	char neighborport[100];

	nachricht = readWholeFile();

	strcpy(neighborip, inet_ntoa(neighborinfo.sin_addr));
	sprintf(neighborport, "%u", ntohs(neighborinfo.sin_port));

	fp = fopen(handle.path, "r");

	while (fscanf(fp, "%[^:]%*c%[^\n]%*c", ip, port) != EOF)
	{
		if(!(dontSendFirstLine >= 1)) {
			if( (neighborip != ip) && (neighborport != port) ) {
				syslog_x(LOG_INFO, "Sende an %s über Port %s\n", ip, port);
				sende_lsp(ip, atoi(port), nachricht);
			}
		}
		else
		{
			dontSendFirstLine--;
		}
		
	}
}

void getMessages() {
	struct sockaddr_in neighborinfo;
	socklen_t len;
	int isequal = 1;
	int neuerNachbar = 0;
	int n;
	char puffer[BUFFERSIZE];
	FILE *fp;
	char * pufferline;
	char fileline[100];

	len = sizeof (neighborinfo);

	syslog_x(LOG_INFO, "Versuche jetzt Nachrichten zu erhalten.\n");
	n = recvfrom ( handle.udp_peer_socket, puffer, BUFFERSIZE, 0, (struct sockaddr *) &neighborinfo, &len );

	syslog_x(LOG_INFO, "Nachricht:\n%swurde von %s an Port %u erhalten.\n",
	puffer, inet_ntoa(neighborinfo.sin_addr), ntohs(neighborinfo.sin_port));
	
	fp = fopen(handle.path, "a+");
	
	if (fp == NULL) {
		syslog_x(LOG_CRIT, "Konnte die Datei nicht öffnen.\n");
	}

	pufferline =strtok(puffer, "\n");

	while(pufferline != NULL) {

		while (fscanf(fp, "%[^\n]%*c", fileline) != EOF)
		{
			if(strcmp(fileline, pufferline) == 0) {
				isequal = 1;
				break;
			} else
			{
				isequal = 0;
			}			
		}
		
		fseek(fp, 0L, SEEK_SET);

		if(isequal == 0) {
			syslog_x(LOG_INFO, "Schreibe jetzt %s n die Datei %s.\n", pufferline, handle.path);
			neuerNachbar = 1;
			isequal = 1;
			fprintf(fp, pufferline);
			fprintf(fp, "\n");
		}

        pufferline = strtok(NULL, "\n");
    }
	fclose(fp);

	if(neuerNachbar == 0) {
		syslog_x(LOG_INFO, "Es wurden keine neuen Nachbarn hinzugefügt. Beende das Programm.\n");
		noNewNeighbors();
	}

	resendIncomingLSP(neighborinfo);
}

int worker()
{
	struct timeval tv;
	fd_set rfds;
	int retval = 0;
	// select shall return once per second
	tv.tv_sec = 100;
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

	}		// end if retval
//fprintf(stderr,"worker leaving %d\n",retval);


	return retval;
}
