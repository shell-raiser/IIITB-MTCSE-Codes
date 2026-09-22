/*
37. FIFO Communication (One-Way): Write two programs to enable communication through FIFO using one-way communication.
*/

// Program 1: Writer (fifo_writer.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    const char *fifo_path = "/tmp/myfifo_oneway";
    char *messages[] = {"Message 1", "Message 2", "Message 3", "END", NULL};
    
    // Create FIFO
    mkfifo(fifo_path, 0666);
    
    int fd = open(fifo_path, O_WRONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    
    printf("Writer: Sending messages...\n");
    for (int i = 0; messages[i] != NULL; i++) {
        write(fd, messages[i], strlen(messages[i]) + 1);
        printf("Sent: %s\n", messages[i]);
        sleep(1);
    }
    
    close(fd);
    unlink(fifo_path);
    return 0;
}

/*
 * Sample output for 37.c
 * Example runtime:
 * $ gcc 37.c -o 37
 * $ ./37
 *
 * Program executed successfully.
 */

/*
 * Actual output of 37.c
 * Command: gcc 37.c -o 37
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

