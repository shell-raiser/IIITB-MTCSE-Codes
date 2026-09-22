/*
19. Process States: Develop a program to initiate a process in distinct states: a. running b. sleeping c. stopped Confirm the current state of the process using the relevant commands.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - demonstrate different states
        printf("Child PID: %d\n", getpid());
        
        // Running state (currently executing)
        printf("State: RUNNING\n");
        sleep(1);
        
        // Sleeping state (waiting for event)
        printf("State: SLEEPING (sleeping for 3 seconds)\n");
        sleep(3);
        
        // Stopped state - send SIGSTOP to self
        printf("State: STOPPED (sending SIGSTOP to self)\n");
        raise(SIGSTOP);
        
        // This won't execute until SIGCONT
        printf("State: RUNNING again (after SIGCONT)\n");
        exit(0);
    } else {
        // Parent - monitor child state via /proc
        printf("Parent PID: %d, Child PID: %d\n", getpid(), pid);
        printf("Check /proc/%d/status for state\n", pid);
        
        // Wait a bit for child to run
        sleep(2);
        
        // Check state
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cat /proc/%d/status | grep State", pid);
        system(cmd);
        
        // Continue child if stopped
        sleep(3);
        kill(pid, SIGCONT);
        waitpid(pid, NULL, 0);
    }
    return 0;
}

/*
 * Sample output for 19.c
 * Example runtime:
 * $ gcc 19.c -o 19
 * $ ./19
 *
 * Program executed successfully.
 */

/*
 * Actual output of 19.c
 * Command: gcc 19.c -o 19
 *
 * State:	S (sleeping)
 * Child PID: 48242
 * State: RUNNING
 * State: SLEEPING (sleeping for 3 seconds)
 * State: STOPPED (sending SIGSTOP to self)
 * State: RUNNING again (after SIGCONT)
 * Parent PID: 48241, Child PID: 48242
 * Check /proc/48242/status for state
 */

