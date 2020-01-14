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

#define SERVER_PORT 1234

#include "msq_header.h"

int main (int argc, char **argv) {
	//Variablen für UDP
	int socketval, retval, i;
	struct sockaddr_in clientinfo, serverinfo;
	struct hostent *h;
	  
	//Client UDP
	
	/* Kommandozeile auswerten */
	if(argc < 3) {
		printf ("Sie müssen mindestens eine IP-Addresse Ihres Nachbarns angeben\n");
		exit (EXIT_FAILURE);
	}
	/* IP-Adresse vom Server überprüfen */
	h = gethostbyname (argv[1]);
	if (h == NULL) {
		printf ("%s: unbekannter Host '%s' \n", argv[0], argv[1] );
		exit (EXIT_FAILURE);
	}
	printf ("%s: sende Daten an '%s' (IP : %s) \n", argv[0], h->h_name, inet_ntoa (*(struct in_addr *) h->h_addr_list[0]) );
	
	serverinfo.sin_family = h->h_addrtype;
	memcpy ( (char *) &serverinfo.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	serverinfo.sin_port = htons (SERVER_PORT);
	
	/* Socket erzeugen */
	socketval = socket (AF_INET, SOCK_DGRAM, 0);
	if (socketval < 0) {
		printf ("%s: Kann Socket nicht öffnen (%s) \n", argv[0], strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	/* Jeden Port bind(en) */
	clientinfo.sin_family = AF_INET;
	clientinfo.sin_addr.s_addr = htonl (INADDR_ANY);
	clientinfo.sin_port = htons (0);
	
	retval = bind ( socketval, (struct sockaddr *) &clientinfo, sizeof (clientinfo) );
	
	if (retval < 0) {
		printf ("%s: Konnte Port nicht bind(en) (%s)\n",
        argv[0], strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	/* Daten senden */
	for (i = 2; i < argc; i++) {
		retval = sendto (socketval, argv[i], strlen (argv[i]) + 1, 0, (struct sockaddr *) &serverinfo, sizeof (serverinfo));
		if (retval < 0) {
			printf ("%s: Konnte Daten nicht senden %d\n", argv[0], i-1 );
			close (socketval);
			exit (EXIT_FAILURE);
		}
	}
  return EXIT_SUCCESS;

	
}