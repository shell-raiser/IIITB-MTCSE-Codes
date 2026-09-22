// 3. File Creation and Descriptor Printing: Write a program that creates a file and prints its file descriptor value using the creat() system call.

#include <stdio.h>      // printf(), perror(), fopen(), fgetc(), putchar(), and fclose().
#include <stdlib.h>     // General-purpose utility definitions; no function is directly used here.
#include <unistd.h>     // write() and close() system calls.
#include <fcntl.h>      // creat() system call and file-opening flags.
#include <sys/types.h>  // System data types used by file-related system calls.
#include <sys/stat.h>   // File permission definitions used by creat().
#include <string.h>     // strlen() used to determine the number of bytes passed to write().

int main() {
    // Create a file using creat() system call
    // creat(pathname, mode) is equivalent to open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode)
    // 'O' stands for "Open" (flags used by open/creat system calls).
    // O_CREAT: Create the file if it does not already exist.
    // O_WRONLY: Open the file for writing only.
    // O_TRUNC: If the file already exists, truncate its size to 0.
    int fd = creat("myfile.txt", 0644);
    
    if (fd < 0) {
        perror("creat"); // prints the descriptive error message to stderr based on the current value of errno
        return 1;
    }
    
    printf("File created successfully!\n");
    printf("File descriptor returned by creat(): %d\n", fd);
    
    // Write something to the file
    const char *msg = "Hello from file descriptor!\n";
    write(fd, msg, strlen(msg));
    
    // Close the file
    close(fd);
    printf("File closed.\n");
    
    // Verify the file exists
    FILE *fp = fopen("myfile.txt", "r");
    if (fp) {
        char ch;
        printf("File contents: ");
        while ((ch = fgetc(fp)) != EOF) {
            putchar(ch);
        }
        fclose(fp);
    }
    
    return 0;
}

/* Commands to run:
   gcc "3. File Creation and Descriptor Printing.c" -o 3_file_creation
   ./3_file_creation
   ls -l myfile.txt
*/
/*
 * Output
 * Command: gcc 3.c -o 3
 *
 * File created successfully!
 * File descriptor returned by creat(): 3
 * File closed.
 * File contents: Hello from file descriptor!
 */

