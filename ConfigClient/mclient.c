/* client_msq.c */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <assert.h>
#include "msq_header.h"

static int ende = 1;
static int setup_client (key_t key, int flag) {
   int res;
   res = msgget (key, flag);
   if (res == -1) {
      printf ("Client-Fehler beim Einrichten"
              " der Message Queues...\n");
      return -1;
   }
   return res;
}
/* Der Client will sich beenden */
static void shutdown_msq (int signr) {
      ende = 0;
}
void show_help(char *name) {
	fprintf(stderr,"Usage HINTS for %s\n", name);
	fprintf(stderr,"%s -m 141.57.9.31 -p 141.57.9.32 -u 141.57.9.33\n", name);
	fprintf(stderr,"-m <string>: eigene ip addresse\n");
	fprintf(stderr,"-p <string>: ip addresse von Partner 1 \n");
	fprintf(stderr,"-q <string>: ip addresse von Partner 2 \n");
	exit(0);
}

void parse_arguments(int argc, char **argv) {
	char c;

	while ((c = getopt(argc, argv, "m:p:q:")) != -1)
		switch (c) {
		case 'm':
			{
				assert(optarg);
				lsp.ip0 = atoll(optarg);
				break;
			}
		case 'p':
			{
				assert(optarg);
				lsp.ip1 = atoll(optarg);
				break;
			}
		case 'q':
			{
				assert(optarg);
				lsp.ip2 = atoll(optarg);
				break;
			}
		case 'h':
		default:
			show_help(argv[0]);
		}
}

// Returns hostname for the local computer 
void checkHostName(int hostname) { 
    if (hostname == -1) 
    { 
        perror("gethostname"); 
        exit(1); 
    } 
} 
  
// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry) { 
    if (hostentry == NULL) 
    { 
        perror("gethostbyname"); 
        exit(1); 
    } 
} 
  
// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
void checkIPbuffer(char *IPbuffer) { 
    if (NULL == IPbuffer) 
    { 
        perror("inet_ntoa"); 
        exit(1); 
    } 
} 

int main (int argc, char **argv) {
	int server_id, client_id;
	int res;
   
	char hostbuffer[256];
    struct hostent *host_entry; 
    int hostname; 
  
    // To retrieve hostname 
    hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    checkHostName(hostname); 
  
    // To retrieve host information 
    host_entry = gethostbyname(hostbuffer); 
    checkHostEntry(host_entry); 
   
   
   
	lsp.ip0 = inet_ntoa(*((struct in_addr*) 
                           host_entry->h_addr_list[0])); 
   
	lsp.ip1 = "141.57.9.23";
	
	lsp.ip2 = "141.57.9.24";
	
	fprintf(stderr, "standardinitalisierung durch");
	
	//might overwrite ip addres
	parse_arguments(argc, argv);
	
	fprintf(stderr, "ip0: %s", lsp.ip0);
	fprintf(stderr, "ip1: %s", lsp.ip1);
	fprintf(stderr, "ip2: %s", lsp.ip2);
	   
	client2server c2s;
	server2client s2c;
	/* Signalhandler für STRG+C einrichten */
	signal (SIGINT, shutdown_msq);
	/* Eine Message Queue zum Server */
	server_id = setup_client (KEY, 0);
	if (server_id < 0)
		return EXIT_FAILURE;
	/* Eine Message Queue für den Client */
	client_id = setup_client (IPC_PRIVATE, PERM | IPC_CREAT);
	if (client_id < 0)
		return EXIT_FAILURE;
	/* Eine Nachricht an den Server versenden */
	c2s.prioritaet = 2;
	sprintf (c2s.message, "%d:Login", client_id);
	res = msgsnd (server_id, &c2s, MSG_LEN, 0);
	if (res == -1) {
      printf ("Konnte keine Nachricht versenden ...\n");
      return EXIT_FAILURE;
   }
   /* Bestätigung des Servers oder Rundschreiben */
   /* von anderen Clients empfangen              */
   res = msgrcv (client_id, &s2c, MSG_LEN, 0, 0);
   if (res == -1) {
      printf ("Fehler beim Erhalt der Nachricht ...\n");
      return EXIT_FAILURE;
   }
   /*Bestätigung oder Rundschreiben auslesen und ausgeben*/
   printf ("%ld: %s\n", s2c.prioritaet, s2c.message);
   while (ende) {
  /* Hier könnte der wichtige Code zur Kommunikation  */
  /* zwischen  den Prozessen geschrieben werden.      */
  /* In diesem Beispiel werden nur die neu erstellten */
  /* Message Queues als neue User ausgegeben. Die     */
  /* Schleife wartet auf das Signal SIGINT == STRG+C  */
      /* Bestätigung oder Rundschreiben empfangen */
      res= msgrcv (client_id, &s2c, MSG_LEN, 0, IPC_NOWAIT);
      if (res != -1) {
         printf ("(%s) von User mit der Message-Queue-ID: "
                 " %ld\n", s2c.message, s2c.prioritaet);
      }
      /* Dauerndes Pollen belastet unnötig die CPU      */
      /* – Eine Bremse (siehe top mit und ohne usleep() */
      usleep( 1000 );
   }
   /* STRG+C also das Signal SIGINT wurde ausgelöst ... */
   c2s.prioritaet = 1;
   sprintf (c2s.message, "%d", client_id);
   res = msgsnd (server_id, &c2s, MSG_LEN, 0);
   if (res == -1) {
      printf ("Konnte keine Nachricht versenden ...\n");
      return EXIT_FAILURE;
   }
   /* Message Queues entfernen */
   msgctl (client_id, IPC_RMID, NULL);
   return EXIT_SUCCESS;
}
