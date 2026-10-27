/*
61. Signal Handling with sigaction()
 a. Catch SIGSEGV
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void sigsegv_handler(int sig, siginfo_t *info, void *context) {
    (void)context;
    printf("Caught SIGSEGV (signal %d) via sigaction\n", sig);
    printf("  Faulting address: %p\n", info->si_addr);
    printf("  si_code         : %d (%s)\n", info->si_code,
           info->si_code == SEGV_MAPERR ? "SEGV_MAPERR - address not mapped" :
           info->si_code == SEGV_ACCERR ? "SEGV_ACCERR - invalid permission" :
                                          "other");
    printf("Note: must not return from a SIGSEGV handler, exiting.\n");
    exit(1);
}

int main(void) {
    struct sigaction sa;
    sa.sa_sigaction = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;   /* gives us siginfo_t -> si_addr */

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("SIGSEGV handler installed with sigaction(SA_SIGINFO).\n");
    printf("Causing a segfault by writing to a NULL pointer...\n");
    fflush(stdout);

    volatile int *p = NULL;
    *p = 42;

    printf("UNREACHABLE\n");
    return 0;
}
/*
 * Output
 * Command: gcc 61a.c -o 61a && ./61a
 *
 * SIGSEGV handler installed with sigaction(SA_SIGINFO).
 * Causing a segfault by writing to a NULL pointer...
 * Caught SIGSEGV (signal 11) via sigaction
 *   Faulting address: (nil)
 *   si_code         : 1 (SEGV_MAPERR - address not mapped)
 * Note: must not return from a SIGSEGV handler, exiting.
 */
