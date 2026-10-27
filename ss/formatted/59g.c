/*
59. Signal Handling using signal()
 g. Catch SIGPROF using the setitimer() system call
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

static volatile sig_atomic_t count = 0;

static void sigprof_handler(int sig) {
    (void)sig;
    count++;
    printf("Caught SIGPROF (ITIMER_PROF) - expiration %d\n", count);
    fflush(stdout);

    if (count >= 3) {
        printf("Exiting after 3 expirations\n");
        exit(0);
    }
}

int main(void) {
    signal(SIGPROF, sigprof_handler);

    struct itimerval timer;
    timer.it_value.tv_sec     = 1;
    timer.it_value.tv_usec    = 0;
    timer.it_interval.tv_sec  = 1;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_PROF, &timer, NULL) == -1) {
        perror("setitimer");
        return 1;
    }

    printf("SIGPROF handler installed; ITIMER_PROF measures user + system CPU time\n");
    printf("Burning CPU so the profiling clock advances...\n");
    fflush(stdout);

    volatile unsigned long sink = 0;
    while (1) {
        for (unsigned long i = 0; i < 1000000UL; i++)
            sink += i;
        if (sink == 0xFFFFFFFFUL)
            sink = 0;
    }

    return 0;
}
/*
 * Output
 * Command: gcc 59g.c -o 59g && ./59g
 *
 * SIGPROF handler installed; ITIMER_PROF measures user + system CPU time
 * Burning CPU so the profiling clock advances...
 * Caught SIGPROF (ITIMER_PROF) - expiration 1
 * Caught SIGPROF (ITIMER_PROF) - expiration 2
 * Caught SIGPROF (ITIMER_PROF) - expiration 3
 * Exiting after 3 expirations
 */
