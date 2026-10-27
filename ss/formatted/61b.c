/*
61. Signal Handling with sigaction()
 b. Catch SIGINT
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t got = 0;

static void sigint_handler(int sig, siginfo_t *info, void *context) {
    (void)info;
    (void)context;
    got = 1;
    const char msg[] = "\nCaught SIGINT (Ctrl+C) via sigaction\n";
    write(STDOUT_FILENO, msg, sizeof msg - 1);  /* async-signal-safe */
    (void)sig;
}

int main(void) {
    struct sigaction sa;
    sa.sa_sigaction = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Process PID: %d\n", getpid());
    printf("SIGINT handler installed with sigaction. Press Ctrl+C...\n");
    fflush(stdout);

    while (!got)
        pause();

    printf("Handler ran; exiting cleanly.\n");
    return 0;
}
/*
 * Output
 * Command: gcc 61b.c -o 61b && ./61b
 *   (press Ctrl+C in the terminal running it)
 *
 * Process PID: 12345
 * SIGINT handler installed with sigaction. Press Ctrl+C...
 * ^C
 * Caught SIGINT (Ctrl+C) via sigaction
 * Handler ran; exiting cleanly.
 */
