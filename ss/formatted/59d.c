/*
59. Signal Handling using signal()
 d. Catch SIGALRM using the alarm() system call
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t expired = 0;

static void sigalrm_handler(int sig) {
    (void)sig;
    expired = 1;
}

int main(void) {
    signal(SIGALRM, sigalrm_handler);

    printf("Installing SIGALRM handler and calling alarm(3)...\n");
    fflush(stdout);

    alarm(3);               /* one-shot: delivers SIGALRM after 3s */

    while (!expired)
        pause();

    printf("Caught SIGALRM after alarm(3) - 3 seconds elapsed\n");
    return 0;
}
/*
 * Output
 * Command: gcc 59d.c -o 59d && time ./59d
 *
 * Installing SIGALRM handler and calling alarm(3)...
 * Caught SIGALRM after alarm(3) - 3 seconds elapsed
 *
 * real    0m3.003s
 */
