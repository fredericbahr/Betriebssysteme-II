/* Skelet für einen DAEMON */
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>

#include "daemon.h"

int main(int argc, char **argv)
{
	int time = 1;

	// if (1 != argc) {
	// 	start_daemon("SMILE");
	// }

	start_daemon("SMILE");
	
	while (1) {
		syslog_x(LOG_NOTICE, "Daemon gestartet ...\n");
		sleep(time);
		syslog_x(LOG_NOTICE,
			 "Daemon läuft bereits %d Sekunden\n", time);
		break;
	}

	syslog_x(LOG_NOTICE, "Daemon gestoppt\n");
	closelog_x();
	return EXIT_SUCCESS;
}
