/*
63. Creating an Orphan Process: Write a program to create an orphan process. Utilize the kill system call to send a SIGKILL signal from the child process to the parent process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child (PID: %d) started, Parent PID: %d\n", getpid(), getppid());
        sleep(2);
        
        // Send SIGKILL to parent
        printf("Child: Sending SIGKILL to parent (PID: %d)\n", getppid());
        if (kill(getppid(), SIGKILL) == 0) {
            printf("Child: SIGKILL sent successfully\n");
        } else {
            perror("Child: kill failed");
        }
        
        // Child becomes orphan (parent killed)
        printf("Child: Now orphan, new parent PID: %d\n", getppid());
        sleep(3);
        printf("Child: Exiting\n");
    } else {
        // Parent process
        printf("Parent (PID: %d) created child (PID: %d)\n", getpid(), pid);
        printf("Parent: Waiting for child...\n");
        
        // Parent will be killed by child's SIGKILL
        // This wait will not return normally
        int status;
        waitpid(pid, &status, 0);
        printf("Parent: This should not print\n");
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 63.c -o 63
 *
 * Child (PID: 48961) started, Parent PID: 48960
 * Child: Sending SIGKILL to parent (PID: 48960)
 * Child: SIGKILL sent successfully
 * Child: Now orphan, new parent PID: 48960
 * Child: Exiting
 */

