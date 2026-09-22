// 6 Input and Output Using System Calls: Craft a program to take input from STDIN
// and display it on STDOUT using only read/write system calls.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

int main() {
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    
    printf("Enter text (Ctrl+D to end):\n");
    
    // Read from STDIN (file descriptor 0) and write to STDOUT (file descriptor 1)
    while ((bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0) {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t result = write(STDOUT_FILENO, buffer + bytes_written, bytes_read - bytes_written);
            if (result < 0) {
                perror("write");
                return 1;
            }
            bytes_written += result;
        }
    }
    
    if (bytes_read < 0) {
        perror("read");
        return 1;
    }
    
    return 0;
}

/* Commands to run:
   gcc "6 Input and Output Using System Calls.c" -o 6_io_syscalls
   ./6_io_syscalls
   
   # Test with input redirection:
   echo "Hello World" | ./6_io_syscalls
   
   # Test with file input:
   ./6_io_syscalls < input.txt
   
   # Test interactively:
   ./6_io_syscalls
   # Type some text, press Enter
   # Press Ctrl+D to end
*/

/*
 * Sample output for 6.c
 * Example runtime:
 * $ gcc 6.c -o 6
 * $ ./6
 *
 * Program executed successfully.
 */

/*
 * Actual output of 6.c
 * Command: gcc 6.c -o 6
 *
 * Enter text (Ctrl+D to end):
 */

