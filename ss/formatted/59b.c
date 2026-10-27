/*
59. Signal Handling using signal()
 b. Catch SIGINT
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void sigint_handler(int sig) {
    printf("\nCaught SIGINT (Ctrl+C), signal number = %d\n", sig);
    exit(0);
}

int main(void) {
    signal(SIGINT, sigint_handler);
    printf("Process PID: %d\n", getpid());
    printf("SIGINT handler installed. Press Ctrl+C to deliver SIGINT...\n");
    fflush(stdout);

    while (1)
        pause();            /* sleep until a signal arrives */

    return 0;
}
/*
 * Output
 * Command: gcc 59b.c -o 59b && ./59b
 *   (press Ctrl+C in the terminal running it)
 *
 * Process PID: 12345
 * SIGINT handler installed. Press Ctrl+C to deliver SIGINT...
 * ^C
 * Caught SIGINT (Ctrl+C), signal number = 2
 */
