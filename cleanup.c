#include "project.h"

void cleanup(void)
{
	printf("GOING 2 FREE MEMORY and FILES graceful!\n");
	// implement here

	if (-1 != handle.web_server_socket){
		syslog_x(LOG_NOTICE, "closing web_server_socket\n");
		close(handle.web_server_socket);
		handle.web_server_socket=-1;
	}

	if (-1 != handle.udp_peer_socket){
		syslog_x(LOG_NOTICE, "closing udp_peer_socket\n");
		close(handle.udp_peer_socket);
		handle.udp_peer_socket=-1;
	}

	syslog_x(LOG_NOTICE, "reached end of cleanup\n");
}
