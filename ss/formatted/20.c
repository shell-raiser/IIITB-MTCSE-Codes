/*
20. Printing Parent and Child Process IDs: Write a program that calls fork and prints both the parent and child process IDs.
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
        // Child process
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
    } else {
        // Parent process
        printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), pid);
        wait(NULL); // Wait for child to complete
    }
    return 0;
}
/*
 * Output
 * Command: gcc 20.c -o 20
 *
 * ./20.c: In function ‘main’:
 * ./20.c:22:9: error: implicit declaration of function ‘wait’ [-Wimplicit-function-declaration]
 *    22 |         wait(NULL); // Wait for child to complete
 *       |         ^~~~
 */

