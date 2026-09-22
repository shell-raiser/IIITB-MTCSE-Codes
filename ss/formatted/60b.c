/*
60. Ignoring and Resetting Signals: Write a program to ignore a SIGINT signal and then reset it to the default action using the signal system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    printf("PID: %d\n", getpid());
    
    // Ignore SIGINT
    printf("Setting SIGINT to SIG_IGN (ignore)...\n");
    signal(SIGINT, SIG_IGN);
    printf("Try pressing Ctrl+C now (should be ignored)\n");
    sleep(5);
    
    // Reset to default
    printf("\nResetting SIGINT to SIG_DFL (default)...\n");
    signal(SIGINT, SIG_DFL);
    printf("Now press Ctrl+C (should terminate program)\n");
    
    while (1) {
        sleep(1);
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 60b.c -o 60b
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

