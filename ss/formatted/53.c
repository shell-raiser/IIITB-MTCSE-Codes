/*
53. Interval Timer Programming: Write separate programs for each time domain to set an interval timer for 10 seconds and 10 microseconds.
a. Use ITIMER_REAL
b. Use ITIMER_VIRTUAL
c. Use ITIMER_PROF
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

volatile int count = 0;

void timer_handler(int sig) {
    count++;
    printf("Timer expired! Count: %d (signal: %d)\n", count, sig);
    if (count >= 5) {
        printf("Exiting after 5 expirations\n");
        exit(0);
    }
}

void setup_timer(int which, int seconds, int microseconds) {
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL); // SIGALRM for REAL, SIGVTALRM for VIRTUAL, SIGPROF for PROF
    
    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = microseconds;
    timer.it_interval.tv_sec = seconds;
    timer.it_interval.tv_usec = microseconds;
    
    if (setitimer(which, &timer, NULL) == -1) {
        perror("setitimer failed");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <a|b|c>\n", argv[0]);
        printf("  a - ITIMER_REAL (10s, 10us)\n");
        printf("  b - ITIMER_VIRTUAL (10s, 10us)\n");
        printf("  c - ITIMER_PROF (10s, 10us)\n");
        exit(1);
    }
    
    int which;
    int signal_num;
    
    switch(argv[1][0]) {
        case 'a':
            which = ITIMER_REAL;
            signal_num = SIGALRM;
            printf("=== ITIMER_REAL ===\n");
            break;
        case 'b':
            which = ITIMER_VIRTUAL;
            signal_num = SIGVTALRM;
            printf("=== ITIMER_VIRTUAL ===\n");
            // Need to handle SIGVTALRM
            signal(SIGVTALRM, timer_handler);
            break;
        case 'c':
            which = ITIMER_PROF;
            signal_num = SIGPROF;
            printf("=== ITIMER_PROF ===\n");
            signal(SIGPROF, timer_handler);
            break;
        default:
            printf("Invalid option\n");
            exit(1);
    }
    
    // Set up signal handler for the appropriate signal
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(signal_num, &sa, NULL);
    
    // Set timer for 10 seconds and 10 microseconds
    struct itimerval timer;
    timer.it_value.tv_sec = 10;
    timer.it_value.tv_usec = 10;
    timer.it_interval.tv_sec = 10;
    timer.it_interval.tv_usec = 10;
    
    if (setitimer(which, &timer, NULL) == -1) {
        perror("setitimer failed");
        exit(1);
    }
    
    printf("Timer set for 10 seconds and 10 microseconds interval\n");
    printf("Waiting for timer expirations...\n");
    
    // Busy wait for VIRTUAL and PROF timers
    if (which == ITIMER_VIRTUAL || which == ITIMER_PROF) {
        while (1) {
            // Busy loop to consume CPU time
        }
    } else {
        // For REAL timer, just pause
        while (1) pause();
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 53.c -o 53
 *
 * Usage: /tmp/ss_run_53_48157 <a|b|c>
 *   a - ITIMER_REAL (10s, 10us)
 *   b - ITIMER_VIRTUAL (10s, 10us)
 *   c - ITIMER_PROF (10s, 10us)
 */

