#include <signal.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);

sighandler_t my_signal (int sig_nr, sighandler_t signalhandler);
void catch_sigint (int sig_nr);
void catch_sigterm (int sig_nr);
void catch_sigalarm (int sig_nr);

int get_sigint_state();
int get_sigterm_state();
int get_sigalarm_state();

