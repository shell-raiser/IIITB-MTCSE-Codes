/*
59. Signal Handling using signal()
 c. Catch SIGFPE
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sigfpe_handler(int sig) {
    printf("Caught SIGFPE (Floating point exception), signal number = %d\n", sig);
    exit(1);
}

int main(void) {
    signal(SIGFPE, sigfpe_handler);
    printf("SIGFPE handler installed. Dividing by zero...\n");
    fflush(stdout);

    volatile int x = 1, y = 0;
    volatile int z = x / y; /* deliberate integer divide-by-zero */
    (void)z;

    printf("UNREACHABLE\n");
    return 0;
}
/*
 * Output
 * Command: gcc 59c.c -o 59c && ./59c
 *
 * SIGFPE handler installed. Dividing by zero...
 * Caught SIGFPE (Floating point exception), signal number = 8
 */
