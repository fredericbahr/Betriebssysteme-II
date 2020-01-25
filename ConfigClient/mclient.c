/* client_msq.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

#include "msq_header.h"

void beende(int sig) {
		exit(EXIT_FAILURE);
}

void show_help(char *name)
{
	fprintf(stderr,"Usage HINTS for %s\n", name);
    fprintf(stderr,"%s 127.0.0.1:24473 141.57.9.22:24474 141.57.9.33:24475 \n", name);
	fprintf(stderr,"Der Aufbau der Argumente ist relative einfach.\n");
	fprintf(stderr,"Sie folgen der Form ip_adresse:portnummer.\n");
	fprintf(stderr,"Das erste Argument ist für den interen Server-Sockeet gedacht, ");
	fprintf(stderr,"daher sollte dieser immer die IP-Adresse des localhsot haben.\n");
	fprintf(stderr,"Die anderen Argumente beziehen sich auf die Nachbarn, mit denen sich verbunden werden soll. \n");
	fprintf(stderr,"Es muss sichergestellt sein, dass wenn auf dem selben Rechner gestartet wird, ");
	fprintf(stderr,"die Portnummer beim Server sowie hier richtig angeben wird und die IP-Adresse dem localhost entspricht.\n");
	exit(0);
}


int main (int argc, char **argv) {
	//Variablen für UDP
	int socketval, retval, i;
	int ende = 1;
	struct sockaddr_in clientinfo, serverinfo;
	struct hostent *h;
	char puffer[1500];
	int anzArgumente;
	char strAnzArgumente[10];
	unsigned short int port;
	  
	//Client UDP
	
	/* Kommandozeile auswerten */
	if(argc < 3) {
		printf ("Sie müssen mindestens eine IP-Addresse Ihres Nachbarns angeben\n");
		showhelp(argv[0]);
	}
	/* IP-Adresse vom Server überprüfen */
	h = gethostbyname (argv[1]);
	if (h == NULL) {
		printf ("%s: unbekannter Host '%s' \n", argv[0], argv[1] );
		exit (EXIT_FAILURE);
	}
	
	port = atoi(argv[2]);

	serverinfo.sin_family = h->h_addrtype;
	memcpy ( (char *) &serverinfo.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	serverinfo.sin_port = htons (port);
	
	/* Socket erzeugen */
	socketval = socket (AF_INET, SOCK_DGRAM, 0);
	if (socketval < 0) {
		printf ("%s: Kann Socket nicht öffnen (%s) \n", argv[0], strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	/* Jeden Port bind(en) */
	clientinfo.sin_family = AF_INET;
	clientinfo.sin_addr.s_addr = htonl (INADDR_ANY);
	clientinfo.sin_port = htons (0); //0 für random port
	
	retval = bind ( socketval, (struct sockaddr *) &clientinfo, sizeof (clientinfo) );
	
	if (retval < 0) {
		printf ("%s: Konnte Port nicht bind(en) (%s)\n",
        argv[0], strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	/* Anzahl an Argumenten übersenden */
	anzArgumente = argc - 3;
	sprintf(strAnzArgumente, "%d", anzArgumente); 
	retval = sendto (socketval, strAnzArgumente, strlen(strAnzArgumente), 0, (struct sockadrr *) &serverinfo, sizeof(serverinfo));
	
	/* Daten senden */
	for (i = 3; i < argc; i++) {
		retval = sendto (socketval, argv[i], strlen (argv[i]) + 1, 0, (struct sockaddr *) &serverinfo, sizeof (serverinfo));
		if (retval < 0) {
			printf ("%s: Konnte Daten nicht senden %d\n", argv[0], i-1 );
			//close (socketval);
			exit (EXIT_FAILURE);
		}
	}
	
	//Auf Antwort warten
	signal(SIGINT, beende);
	while (ende) {
		/* Puffer initialisieren */
		memset (puffer, 0, 255);
		/* Nachrichten empfangen */
		int len = sizeof(serverinfo);
		
		retval = recvfrom (socketval, &puffer, 1500, 0, (struct sockaddr *) &serverinfo, &len);
		
		if (retval < 0) {
		   perror("Error: ");
		   exit(EXIT_FAILURE);
		}

		/* Erhaltene Nachricht ausgeben */
		printf("%s", puffer);
	
	}
  return EXIT_SUCCESS;	
}
