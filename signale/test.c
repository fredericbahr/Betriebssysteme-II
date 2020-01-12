#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "signale.h"

void *ptr=NULL; // global variable to handle all open files + memory

void bye(void)
{
  printf("GOING 2 FREE MEMORY and FILES graceful!\n");
  if (NULL != ptr) free(ptr);
  printf("DONE.\n");
}


int main (void) {

	atexit(bye);
	alarm(5); // prog shall not nrun longer than X seconds

   my_signal (SIGTERM, catch_sigterm);
   my_signal (SIGINT, catch_sigint);
   my_signal (SIGALRM, catch_sigalarm);

   ptr = (void *) malloc(4);

   fprintf(stderr,"Prozessid: %d\n\n",getpid());

   do {
	if (
		(0 == get_sigint_state()) ||
		(0 == get_sigterm_state()) 
	   ) break; // end if and exit do while
usleep(1000*200);
      printf ("Ping-");
usleep(1000*800);
      printf ("Pong\n");
		//fprintf(stderr,"%d %d\n",get_sigint_state(),get_sigterm_state());
   } while (1);
   return EXIT_SUCCESS;
}
