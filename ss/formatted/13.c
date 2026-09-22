// 13. Waiting for STDIN Using Select: Develop a program to wait for input from STDIN
// for 10 seconds using select. Include proper print statements to verify data availability
// within the specified time.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

int main() {
    fd_set read_fds;
    struct timeval timeout;
    int result;
    char buffer[1024];

    printf("Waiting for input from STDIN for 10 seconds...\n");
    printf("Type something and press Enter, or wait for timeout.\n\n");

    // Initialize the file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    // Set timeout to 10 seconds
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    printf("select() called with 10 second timeout...\n");
    fflush(stdout);

    // Wait for input
    result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

    if (result < 0) {
        perror("select");
        return 1;
    } else if (result == 0) {
        printf("\nTimeout! No input received within 10 seconds.\n");
    } else {
        // Check if STDIN is ready
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            printf("\nData is available on STDIN!\n");
            
            // Read the input
            ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("Input received (%zd bytes): %s", bytes, buffer);
            } else if (bytes == 0) {
                printf("EOF received (Ctrl+D)\n");
            } else {
                perror("read");
            }
        }
    }

    return 0;
}

/* Commands to run:
   gcc "13 Waiting for STDIN Using Select.c" -o 13_select_stdin
   ./13_select_stdin
   
   # Test 1: Provide input within 10 seconds
   echo "Hello" | ./13_select_stdin
   
   # Test 2: Let it timeout (don't type anything)
   ./13_select_stdin
   # Wait 10 seconds
   
   # Test 3: Type input interactively
   ./13_select_stdin
   # Type "test" and press Enter
*/
/*
 * Output
 * Command: gcc 13.c -o 13
 *
 * Waiting for input from STDIN for 10 seconds...
 * Type something and press Enter, or wait for timeout.
 *
 * select() called with 10 second timeout...
 *
 * Data is available on STDIN!
 * EOF received (Ctrl+D)
 */

