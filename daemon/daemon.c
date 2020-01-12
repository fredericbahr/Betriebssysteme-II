#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#define _DEFAULT_SOURCE 1

int use_syslog = -1;

int syslog_to_console(int value)
{
	int old = use_syslog;
	use_syslog = value;
	return old;
}

int syslog_x(int prio, const char *fmt, ...)
{
	int r = 0;
	va_list ap;
	va_start(ap, fmt);
	if (0 == use_syslog)
		vsyslog(prio, fmt, ap);
	else
		r = vprintf(fmt, ap);
	va_end(ap);
	return r;
}

void start_daemon(const char *ident)
{
	int i;
	pid_t pid;

	if ((pid = fork()) != 0) {
		syslog_x(LOG_INFO, "Hängt es an dem Fork?\n");
		syslog_x(LOG_INFO, "pid: %d\n", pid);
		exit(EXIT_FAILURE);
	}
	if (setsid() < 0) {
		fprintf(stderr, "setsid failed\n");
		exit(EXIT_FAILURE);
	}
	//handle_signal (SIGHUP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	if ((pid = fork()) != 0)
		exit(EXIT_FAILURE);
	chdir("/");
	umask(0);
	for (i = sysconf(_SC_OPEN_MAX); i > 0; i--)
		close(i);

	use_syslog = 0;
	openlog(ident, LOG_PID | LOG_CONS | LOG_NDELAY, LOG_LOCAL0);
	syslog_x(LOG_INFO, "Fertig mit Daemonstart\n");
}

void closelog_x(void)
{
	closelog();
}
