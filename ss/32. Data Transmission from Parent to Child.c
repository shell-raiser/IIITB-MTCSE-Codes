/*
32. Data Transmission from Parent to Child: Develop a simple program to send data from the parent process to the child process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    char buffer[256];
    char *messages[] = {
        "Message 1 from parent",
        "Message 2 from parent", 
        "Message 3 from parent",
        NULL
    };
    
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child - receive messages
        close(fd[1]);
        printf("Child (PID: %d) waiting for messages...\n", getpid());
        while (read(fd[0], buffer, sizeof(buffer)) > 0) {
            printf("Child received: %s\n", buffer);
        }
        printf("Child: No more data\n");
        close(fd[0]);
    } else {
        // Parent - send messages
        close(fd[0]);
        printf("Parent (PID: %d) sending messages...\n", getpid());
        for (int i = 0; messages[i] != NULL; i++) {
            write(fd[1], messages[i], strlen(messages[i]) + 1);
            sleep(1);
        }
        close(fd[1]);
        wait(NULL);
        printf("Parent: All messages sent\n");
    }
    
    return 0;
}
