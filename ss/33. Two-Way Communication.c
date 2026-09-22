/*
33. Two-Way Communication: Write a program to send and receive data between the parent and child processes using two-way communication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd1[2]; // Parent to child
    int fd2[2]; // Child to parent
    char buffer[256];
    
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child
        close(fd1[1]); // Close parent->child write
        close(fd2[0]); // Close child->parent read
        
        while (read(fd1[0], buffer, sizeof(buffer)) > 0) {
            printf("Child received: %s\n", buffer);
            
            // Send response back
            char response[256];
            snprintf(response, sizeof(response), "ACK: %s", buffer);
            write(fd2[1], response, strlen(response) + 1);
        }
        
        close(fd1[0]);
        close(fd2[1]);
    } else {
        // Parent
        close(fd1[0]); // Close parent->child read
        close(fd2[1]); // Close child->parent write
        
        char *messages[] = {"Hello", "How are you?", "Goodbye", NULL};
        
        for (int i = 0; messages[i] != NULL; i++) {
            printf("Parent sending: %s\n", messages[i]);
            write(fd1[1], messages[i], strlen(messages[i]) + 1);
            
            // Read response
            read(fd2[0], buffer, sizeof(buffer));
            printf("Parent received response: %s\n", buffer);
            sleep(1);
        }
        
        close(fd1[1]);
        close(fd2[0]);
        wait(NULL);
    }
    
    return 0;
}
