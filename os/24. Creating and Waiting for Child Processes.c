/*
24. Creating and Waiting for Child Processes: Write a program to create three child processes. The parent process should wait for a specific child process using the waitpid system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pids[3];
    
    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        } else if (pids[i] == 0) {
            // Child process
            printf("Child %d (PID: %d) sleeping for %d seconds\n", i+1, getpid(), (i+1)*2);
            sleep((i+1) * 2);
            printf("Child %d (PID: %d) exiting\n", i+1, getpid());
            exit(i+1); // Exit with different codes
        }
    }
    
    // Parent waits for specific child (second child)
    printf("Parent waiting for child 2 (PID: %d) using waitpid\n", pids[1]);
    int status;
    pid_t waited = waitpid(pids[1], &status, 0);
    
    if (waited == pids[1]) {
        if (WIFEXITED(status)) {
            printf("Child 2 exited with status: %d\n", WEXITSTATUS(status));
        }
    }
    
    // Wait for remaining children
    for (int i = 0; i < 3; i++) {
        if (pids[i] != pids[1]) {
            waitpid(pids[i], &status, 0);
            printf("Child %d reaped\n", i+1);
        }
    }
    
    return 0;
}
