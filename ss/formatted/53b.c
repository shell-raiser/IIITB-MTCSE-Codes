/*
53. Interval Timer Programming: Write separate programs for each time domain to set an interval timer for 10 seconds and 10 microseconds.
b. Use ITIMER_VIRTUAL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

static volatile sig_atomic_t count = 0;

static void timer_handler(int sig) {
    (void)sig;
    count++;

    char buf[64];
    int n = snprintf(buf, sizeof buf, "Timer expired! Count: %d\n", (int)count);
    if (write(STDOUT_FILENO, buf, n) == -1) { /* nothing useful to do */ }

    if (count >= 5) {
        const char msg[] = "Exiting after 5 expirations\n";
        if (write(STDOUT_FILENO, msg, sizeof msg - 1) == -1) { }
        _exit(0);
    }
}

int main(void) {
    printf("=== ITIMER_VIRTUAL (user CPU time) ===\n");

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGVTALRM, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    struct itimerval timer;
    memset(&timer, 0, sizeof timer);
    timer.it_value.tv_sec     = 10;
    timer.it_value.tv_usec    = 10;
    timer.it_interval.tv_sec  = 10;
    timer.it_interval.tv_usec = 10;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        perror("setitimer");
        return 1;
    }

    printf("ITIMER_VIRTUAL set for 10 seconds and 10 microseconds interval\n");
    printf("Signal: SIGVTALRM - counts user CPU time only\n");
    printf("Burning CPU so the virtual clock advances...\n");
    fflush(stdout);

    /*
     * ITIMER_VIRTUAL only advances while the process is running in user
     * mode, so we must keep the CPU busy. sleep()/pause() would never
     * fire this timer.
     */
    volatile unsigned long sink = 0;
    while (1) {
        for (unsigned long i = 0; i < 1000000UL; i++) {
            sink += i;
        }
        if (sink == 0xFFFFFFFFUL) {
            sink = 0;
        }
    }

    return 0;
}
/*
 * Output
 * Command: gcc 53b.c -o 53b && ./53b
 *
 * === ITIMER_VIRTUAL (user CPU time) ===
 * ITIMER_VIRTUAL set for 10 seconds and 10 microseconds interval
 * Signal: SIGVTALRM - counts user CPU time only
 * Burning CPU so the virtual clock advances...
 * Timer expired! Count: 1
 * Timer expired! Count: 2
 * Timer expired! Count: 3
 * Timer expired! Count: 4
 * Timer expired! Count: 5
 * Exiting after 5 expirations
 */
