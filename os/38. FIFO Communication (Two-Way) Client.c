/*
38. FIFO Communication (Two-Way): Write two programs to establish communication through FIFO using two-way communication.
*/

// Program 2: Client (fifo_client.c)
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
    
    // Wait for server to create FIFOs
    while (access(fifo_to_client, F_OK) != 0 || access(fifo_from_client, F_OK) != 0) {
        usleep(100000);
    }
    
    // Open FIFOs (reverse order to avoid deadlock)
    int fd_read = open(fifo_to_client, O_RDONLY);
    int fd_write = open(fifo_from_client, O_WRONLY);
    
    if (fd_read < 0 || fd_write < 0) {
        perror("open failed");
        exit(1);
    }
    
    printf("Client: Connected to server\n");
    
    char *messages[] = {"Hello", "How are you?", "Goodbye", "exit", NULL};
    
    for (int i = 0; messages[i] != NULL; i++) {
        // Send message
        write(fd_write, messages[i], strlen(messages[i]) + 1);
        printf("Client sent: %s\n", messages[i]);
        
        // Read response
        int n = read(fd_read, buffer, sizeof(buffer));
        if (n > 0) {
            printf("Client received: %s\n", buffer);
        }
        sleep(1);
    }
    
    close(fd_write);
    close(fd_read);
    return 0;
}
