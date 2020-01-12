#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);
volatile sig_atomic_t sigint = 1;
volatile sig_atomic_t sigterm = 1;
volatile sig_atomic_t sigalarm = 1;

sighandler_t my_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction neu_sig, alt_sig;
   neu_sig.sa_handler = signalhandler;
   sigemptyset (&neu_sig.sa_mask);
   neu_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
      return SIG_ERR;
   return alt_sig.sa_handler;
}

void catch_sigint (int sig_nr) {
   sigint--;
   printf("int\n");
   my_signal (sig_nr, catch_sigint); 
}
int get_sigint_state() {
	return sigint;
}

void catch_sigterm (int sig_nr) {
   sigterm--;
   printf("term\n");
   my_signal (sig_nr, catch_sigterm); 
}
int get_sigterm_state() {
	return sigterm;
}

void catch_sigalarm (int sig_nr) {
   sigterm--;
   printf("alarm\n");
   my_signal (sig_nr, catch_sigterm); 
}
int get_sigalarm_state() {
	return sigterm;
}

