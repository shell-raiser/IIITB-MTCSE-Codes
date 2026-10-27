/*
59. Signal Handling using signal()
 f. Catch SIGVTALRM using the setitimer() system call
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

static volatile sig_atomic_t count = 0;

static void sigvtalrm_handler(int sig) {
    (void)sig;
    count++;
    printf("Caught SIGVTALRM (ITIMER_VIRTUAL) - expiration %d\n", count);
    fflush(stdout);

    if (count >= 3) {
        printf("Exiting after 3 expirations\n");
        exit(0);
    }
}

int main(void) {
    signal(SIGVTALRM, sigvtalrm_handler);

    struct itimerval timer;
    timer.it_value.tv_sec     = 1;
    timer.it_value.tv_usec    = 0;
    timer.it_interval.tv_sec  = 1;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        perror("setitimer");
        return 1;
    }

    printf("SIGVTALRM handler installed; ITIMER_VIRTUAL measures user CPU time\n");
    printf("Burning CPU so the virtual clock advances...\n");
    fflush(stdout);

    /*
     * ITIMER_VIRTUAL only advances while the process executes in user mode.
     * pause()/sleep() would never fire it, so keep the CPU busy.
     */
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
 * Command: gcc 59f.c -o 59f && ./59f
 *
 * SIGVTALRM handler installed; ITIMER_VIRTUAL measures user CPU time
 * Burning CPU so the virtual clock advances...
 * Caught SIGVTALRM (ITIMER_VIRTUAL) - expiration 1
 * Caught SIGVTALRM (ITIMER_VIRTUAL) - expiration 2
 * Caught SIGVTALRM (ITIMER_VIRTUAL) - expiration 3
 * Exiting after 3 expirations
 */
