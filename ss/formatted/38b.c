/*
38. FIFO Communication (Two-Way): Write two programs to establish communication through FIFO using two-way communication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define CLIENT_TO_SERVER "/tmp/fifo_client_to_server"
#define SERVER_TO_CLIENT "/tmp/fifo_server_to_client"

int main(void) {
    while (access(CLIENT_TO_SERVER, F_OK) != 0 || access(SERVER_TO_CLIENT, F_OK) != 0) {
        usleep(100000);
    }

    int req_fd = open(CLIENT_TO_SERVER, O_WRONLY);
    int resp_fd = open(SERVER_TO_CLIENT, O_RDONLY);
    if (req_fd < 0 || resp_fd < 0) {
        perror("open");
        return 1;
    }

    const char *messages[] = {"Hello server", "How are you?", "exit", NULL};
    for (int i = 0; messages[i] != NULL; i++) {
        write(req_fd, messages[i], strlen(messages[i]) + 1);
        printf("Client sent: %s\n", messages[i]);

        char buffer[256];
        ssize_t n = read(resp_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Client received: %s\n", buffer);
        }
    }

    close(req_fd);
    close(resp_fd);
    return 0;
}
/*
 * Output
 * Command: gcc 38b.c -o 38b && ./38b
 *
 * Client sent: Hello server
 * Client received: ACK: Hello server
 * Client sent: How are you?
 * Client received: ACK: How are you?
 * Client sent: exit
 * Client received: Bye from server
 */

