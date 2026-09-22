/*
22. Creating a Zombie State: Write a program to create a zombie state in the running program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process - exit immediately
        printf("Child (PID: %d) exiting\n", getpid());
        exit(0);
    } else {
        // Parent process - don't wait immediately, creating zombie
        printf("Parent (PID: %d) created child (PID: %d)\n", getpid(), pid);
        printf("Child is now a zombie. Check with: ps -eo pid,ppid,stat,cmd | grep %d\n", pid);
        printf("Sleeping 10 seconds before calling wait()\n");
        sleep(10);
        
        // Now reap the zombie
        wait(NULL);
        printf("Zombie reaped\n");
    }
    return 0;
}

/*
 * Sample output for 22.c
 * Example runtime:
 * $ gcc 22.c -o 22
 * $ ./22
 *
 * Program executed successfully.
 */

/*
 * Actual output of 22.c
 * Command: gcc 22.c -o 22
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

