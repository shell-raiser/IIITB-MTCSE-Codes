/*
31. Pipe Creation and Communication: Write a simple program to create a pipe, write to the pipe, read from the pipe, and display the content on the monitor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
