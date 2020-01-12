void start_daemon(const char *ident);
int syslog_to_console(int value);
int syslog_x(int prio, const char *fmt, ...);
void closelog_x(void);
