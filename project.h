#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
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
#define PERM 066
#define MSG_LEN 1024

typedef struct {
   long prioritaet;
   char message[MSG_LEN];
} client2server;

typedef struct {
   long prioritaet;
   char message[MSG_LEN];
} server2client;

struct handle_struct{
	unsigned short int web_portnummer;
	unsigned short int udp_portnummer;
	int web_server_socket;
	int udp_peer_socket;
	char lastmsg[BUFFERSIZE];
	fd_set rfds;
    int max_socket;
	unsigned short int secret;
};
struct handle_struct handle;

/* init.c functions */
int init_everything(int argc, char **argv);

/* cleanup.c functions */
void cleanup(void);

/* worker.c functions */
int worker();

static int setup_client (key_t key, int flag);

static int setup_server( key_t key, int flag );

