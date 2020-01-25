/* Skelet für einen DAEMON */
#include "project.h"

int main(int argc, char **argv){
// INIT EVERYTHING
	if (0 != init_everything(argc, argv) ) {
		syslog_x(LOG_CRIT, "Daemon INIT failed. ");
		return EXIT_FAILURE;
	} // end if init_everything
	syslog_x(LOG_NOTICE, "Daemon gestartet. PID: %d\n", getpid());

// START WORKING
	sleep(5);
	sendInitialLSP();
	getMessages();
	do {

		// call functions
		//worker();

	} while ((1 == get_sigint_state()) && (1 == get_sigterm_state()));

// CLEAN UP EVERYTHING

	syslog_x(LOG_NOTICE, "Daemon gestoppt\n");
	closelog_x();
	return EXIT_SUCCESS;
}
