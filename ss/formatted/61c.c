/*
61. Signal Handling with sigaction()
 c. Catch SIGFPE
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void sigfpe_handler(int sig, siginfo_t *info, void *context) {
    (void)sig;
    (void)context;
    printf("Caught SIGFPE (Floating point exception) via sigaction\n");
    printf("  si_code: %d (%s)\n", info->si_code,
           info->si_code == FPE_INTDIV ? "FPE_INTDIV - integer divide by zero" :
           info->si_code == FPE_INTOVF ? "FPE_INTOVF - integer overflow" :
           info->si_code == FPE_FLTDIV ? "FPE_FLTDIV - floating divide by zero" :
                                         "other");
    exit(1);
}

int main(void) {
    struct sigaction sa;
    sa.sa_sigaction = sigfpe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGFPE, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("SIGFPE handler installed with sigaction. Dividing by zero...\n");
    fflush(stdout);

    volatile int x = 1, y = 0;
    volatile int z = x / y;
    (void)z;

    printf("UNREACHABLE\n");
    return 0;
}
/*
 * Output
 * Command: gcc 61c.c -o 61c && ./61c
 *
 * SIGFPE handler installed with sigaction. Dividing by zero...
 * Caught SIGFPE (Floating point exception) via sigaction
 *   si_code: 1 (FPE_INTDIV - integer divide by zero)
 */
