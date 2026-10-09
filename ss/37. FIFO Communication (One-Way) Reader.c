/*
37. FIFO Communication (One-Way): Write two programs to enable communication through FIFO using one-way communication.
*/

// Program 2: Reader (fifo_reader.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    const char *fifo_path = "/tmp/myfifo_oneway";
    char buffer[256];
    
    // Wait for FIFO to exist
    while (access(fifo_path, F_OK) != 0) {
        usleep(100000);
    }
    
    int fd = open(fifo_path, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    
    printf("Reader: Waiting for messages...\n");
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        if (strcmp(buffer, "END") == 0) {
            printf("Received END signal\n");
            break;
        }
        printf("Received: %s\n", buffer);
    }
    
    close(fd);
    return 0;
}
