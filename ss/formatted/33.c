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

            char response[256];
            snprintf(response, sizeof(response), "ACK: ");
            strncat(response, buffer, sizeof(response) - strlen(response) - 1);
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

/*
 * Sample output for 33.c
 * Example runtime:
 * $ gcc 33.c -o 33
 * $ ./33
 *
 * Program executed successfully.
 */

/*
 * Actual output of 33.c
 * Command: gcc 33.c -o 33
 *
 * Child received: Hello
 * Child received: How are you?
 * Child received: Goodbye
 * Parent sending: Hello
 * Parent received response: ACK: Hello
 * Parent sending: How are you?
 * Parent received response: ACK: How are you?
 * Parent sending: Goodbye
 * Parent received response: ACK: Goodbye
 */

