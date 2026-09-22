/*
35. Counting Directories with dup2: Write a program to find the total number of directories in the present working directory using ls -l | grep ^d | wc. Implement the solution using only dup2.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd1[2]; // ls -> grep
    int fd2[2]; // grep -> wc
    
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    // Process 1: ls -l
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        execlp("ls", "ls", "-l", NULL);
        perror("execlp ls failed");
        exit(1);
    }
    
    // Process 2: grep ^d
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd1[1]);
        close(fd2[0]);
        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);
        close(fd1[0]);
        close(fd2[1]);
        execlp("grep", "grep", "^d", NULL);
        perror("execlp grep failed");
        exit(1);
    }
    
    // Process 3: wc -l
    pid_t pid3 = fork();
    if (pid3 == 0) {
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[1]);
        dup2(fd2[0], STDIN_FILENO);
        close(fd2[0]);
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc failed");
        exit(1);
    }
    
    // Parent: close all pipe ends
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    
    // Wait for all children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    
    return 0;
}

/*
 * Sample output for 35.c
 * Example runtime:
 * $ gcc 35.c -o 35
 * $ ./35
 *
 * Program executed successfully.
 */

/*
 * Actual output of 35.c
 * Command: gcc 35.c -o 35
 *
 * 0
 */

