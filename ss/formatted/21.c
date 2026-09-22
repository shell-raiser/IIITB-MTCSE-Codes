/*
21. File Writing by Parent and Child Processes: Develop a program that opens a file, calls fork, and allows both the child and parent processes to write to the file. Examine the output of the file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child writes
        char *msg = "Child writing to file\n";
        write(fd, msg, strlen(msg));
        printf("Child wrote to file\n");
    } else {
        // Parent writes
        char *msg = "Parent writing to file\n";
        write(fd, msg, strlen(msg));
        printf("Parent wrote to file\n");
        wait(NULL);
    }
    
    close(fd);
    return 0;
}

/*
 * Sample output for 21.c
 * Example runtime:
 * $ gcc 21.c -o 21
 * $ ./21
 *
 * Program executed successfully.
 */

/*
 * Actual output of 21.c
 * Command: gcc 21.c -o 21
 *
 * Child wrote to file
 * Parent wrote to file
 */

