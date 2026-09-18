/*
38. FIFO Communication (Two-Way): Write two programs to establish communication through FIFO using two-way communication.
*/

// Program 1: Server (fifo_server.c)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    const char *fifo_to_client = "/tmp/fifo_to_client";
    const char *fifo_from_client = "/tmp/fifo_from_client";
    char buffer[256];
    
    mkfifo(fifo_to_client, 0666);
    mkfifo(fifo_from_client, 0666);
    
    // Open FIFOs
    int fd_write = open(fifo_to_client, O_WRONLY);
    int fd_read = open(fifo_from_client, O_RDONLY);
    
    if (fd_write < 0 || fd_read < 0) {
        perror("open failed");
        exit(1);
    }
    
    printf("Server: Connected to client\n");
    
    while (1) {
        // Read from client
        int n = read(fd_read, buffer, sizeof(buffer));
        if (n <= 0) break;
        
        printf("Server received: %s\n", buffer);
        
        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested exit\n");
            break;
        }
        
        // Send response
        char response[256];
        snprintf(response, sizeof(response), "Server echo: %s", buffer);
        write(fd_write, response, strlen(response) + 1);
    }
    
    close(fd_write);
    close(fd_read);
    unlink(fifo_to_client);
    unlink(fifo_from_client);
    return 0;
}
