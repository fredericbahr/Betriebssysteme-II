#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "daemon/daemon.h"
#include "signale/signale.h"

#define BUFFERSIZE 1500

struct handle_struct{
	unsigned short int web_portnummer;
	unsigned short int udp_portnummer;
	int web_server_socket;
	int udp_peer_socket;
	char lastmsg[BUFFERSIZE];
	fd_set rfds;
    int max_socket;
	char * path;
};
struct handle_struct handle;

/* init.c functions */
int init_everything(int argc, char **argv);

/* cleanup.c functions */
void cleanup(void);

/* worker.c functions */
int worker();

void beendeServer();

void beende(int sig);

void newNeighbors();

int sende_lsp(char *dest, unsigned short int port, char *nachricht);

char * readWholeFile();

void sendInitialLSP();

void resendIncomingLSP(struct sockaddr_in neighborinfo);

void getMessages();

void getOwnIP_Port(FILE * fp);