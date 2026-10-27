/*
64. Signal SIGSTOP Handling: Create two programs: the first program awaits
the SIGSTOP signal, while the second program sends the signal using the kill
system call. Determine whether the first program successfully catches the
signal or not.

Conclusion: SIGSTOP CANNOT be caught, blocked, or ignored. sigaction()
fails with EINVAL, and the process is stopped by the kernel instead of
running any handler.

Program 1 of 2: receiver (waits for SIGSTOP) - prints its own PID.
Companion program: 64b.c (sender).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("Receiver PID: %d\n", getpid());
    printf("SIGSTOP cannot be caught, blocked, or ignored.\n");
    printf("Attempting to install a handler (expected to fail)...\n");
    fflush(stdout);

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGSTOP, &sa, NULL) == -1)
        printf("sigaction(SIGSTOP) failed as expected: %s\n", strerror(errno));
    else
        printf("Unexpected: sigaction(SIGSTOP) succeeded\n");

    printf("\nNow run, in another terminal:\n");
    printf("  ./64b %d\n", getpid());
    printf("Waiting (this process will be STOPPED, not handled)...\n");
    fflush(stdout);

    /* If SIGSTOP is delivered we are suspended here and never resume
     * unless someone sends SIGCONT. */
    for (;;) pause();

    return 0;
}
/*
 * Output
 * Command: gcc 64a.c -o 64a && ./64a
 *
 * Receiver PID: 12345
 * SIGSTOP cannot be caught, blocked, or ignored.
 * Attempting to install a handler (expected to fail)...
 * sigaction(SIGSTOP) failed as expected: Invalid argument
 *
 * Now run, in another terminal:
 *   ./64b 12345
 * Waiting (this process will be STOPPED, not handled)...
 *
 * --- after ./64b 12345 in another terminal ---
 * (no handler output; process state becomes 'T' per: ps -o stat= -p 12345)
 */
