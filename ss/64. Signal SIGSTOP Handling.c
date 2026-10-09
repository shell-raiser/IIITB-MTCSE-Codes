/*
64. Signal SIGSTOP Handling: Create two programs: the first program awaits the SIGSTOP signal, while the second program sends the signal using the kill system call. Determine whether the first program successfully catches the signal or not.
*/

// Program 1: Waits for SIGSTOP (sigstop_receiver.c)
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigstop_handler(int sig) {
    // NOTE: SIGSTOP CANNOT BE CAUGHT OR IGNORED!
    // This handler will NEVER be called
    printf("This will never print - SIGSTOP cannot be caught!\n");
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "sender") == 0) {
        // This is the sender program (program 2)
        if (argc < 3) {
            printf("Usage: %s sender <pid>\n", argv[0]);
            exit(1);
        }
        pid_t target = atoi(argv[2]);
        printf("Sender: Sending SIGSTOP to PID %d\n", target);
        if (kill(target, SIGSTOP) == 0) {
            printf("Sender: SIGSTOP sent successfully\n");
        } else {
            perror("Sender: kill failed");
        }
        return 0;
    }
    
    // Receiver program
    printf("Receiver PID: %d\n", getpid());
    printf("SIGSTOP CANNOT be caught, blocked, or ignored!\n");
    printf("Attempting to install handler (will not work)...\n");
    
    struct sigaction sa;
    sa.sa_handler = sigstop_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    // This will fail - SIGSTOP cannot be caught
    if (sigaction(SIGSTOP, &sa, NULL) == -1) {
        perror("sigaction failed (expected)");
    }
    
    printf("Waiting for SIGSTOP (send from another terminal: kill -STOP %d)\n", getpid());
    printf("Then resume with: kill -CONT %d\n", getpid());
    
    while (1) {
        pause();
    }
    
    return 0;
}
