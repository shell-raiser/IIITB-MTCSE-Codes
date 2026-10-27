/*
59. Signal Handling using signal()
 e. Catch SIGALRM using the setitimer() system call
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

static volatile sig_atomic_t count = 0;

static void sigalrm_handler(int sig) {
    (void)sig;
    count++;
    printf("Caught SIGALRM (setitimer ITIMER_REAL) - expiration %d\n", count);
    fflush(stdout);

    if (count >= 3) {
        printf("Exiting after 3 expirations\n");
        exit(0);
    }
}

int main(void) {
    signal(SIGALRM, sigalrm_handler);

    struct itimerval timer;
    timer.it_value.tv_sec     = 1;   /* first expiration after 1s   */
    timer.it_value.tv_usec    = 0;
    timer.it_interval.tv_sec  = 1;   /* then repeat every 1s        */
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        return 1;
    }

    printf("SIGALRM handler installed; ITIMER_REAL fires every 1 second\n");
    fflush(stdout);

    while (1)
        pause();

    return 0;
}
/*
 * Output
 * Command: gcc 59e.c -o 59e && ./59e
 *
 * SIGALRM handler installed; ITIMER_REAL fires every 1 second
 * Caught SIGALRM (setitimer ITIMER_REAL) - expiration 1
 * Caught SIGALRM (setitimer ITIMER_REAL) - expiration 2
 * Caught SIGALRM (setitimer ITIMER_REAL) - expiration 3
 * Exiting after 3 expirations
 */
