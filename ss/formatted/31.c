/*
31. Pipe Creation and Communication: Write a simple program to create a pipe, write to the pipe, read from the pipe, and display the content on the monitor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    char buffer[100];
    char *msg = "Hello through pipe!";
    
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child - read from pipe
        close(fd[1]); // Close write end
        read(fd[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(fd[0]);
    } else {
        // Parent - write to pipe
        close(fd[0]); // Close read end
        write(fd[1], msg, strlen(msg) + 1);
        printf("Parent sent: %s\n", msg);
        close(fd[1]);
        wait(NULL);
    }
    
    return 0;
}

/*
 * Sample output for 31.c
 * Example runtime:
 * $ gcc 31.c -o 31
 * $ ./31
 *
 * Program executed successfully.
 */

/*
 * Actual output of 31.c
 * Command: gcc 31.c -o 31
 *
 * ./31.c: In function ‘main’:
 * ./31.c:37:9: error: implicit declaration of function ‘wait’ [-Wimplicit-function-declaration]
 *    37 |         wait(NULL);
 *       |         ^~~~
 */

