/*
62. Signal Handling with sigaction: Write a program to ignore a SIGINT signal and then reset it to the default action using the sigaction system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    printf("PID: %d\n", getpid());
    
    // Ignore SIGINT using sigaction
    struct sigaction sa_ignore, sa_default, sa_old;
    
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    
    printf("Setting SIGINT to SIG_IGN using sigaction...\n");
    sigaction(SIGINT, &sa_ignore, &sa_old);
    printf("Old handler saved. Try pressing Ctrl+C (should be ignored)\n");
    sleep(5);
    
    // Reset to default
    sa_default.sa_handler = SIG_DFL;
    sigemptyset(&sa_default.sa_mask);
    sa_default.sa_flags = 0;
    
    printf("\nResetting SIGINT to SIG_DFL using sigaction...\n");
    sigaction(SIGINT, &sa_default, NULL);
    printf("Now press Ctrl+C (should terminate program)\n");
    
    while (1) {
        sleep(1);
    }
    
    return 0;
}

/*
 * Sample output for 62b.c
 * Example runtime:
 * $ gcc 62b.c -o 62b
 * $ ./62b
 *
 * SIGINT reset to default
 */

/*
 * Actual output of 62b.c
 * Command: gcc 62b.c -o 62b
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

