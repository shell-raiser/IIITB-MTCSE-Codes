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
    if (mkfifo(CLIENT_TO_SERVER, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo client_to_server");
        return 1;
    }
    if (mkfifo(SERVER_TO_CLIENT, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo server_to_client");
        unlink(CLIENT_TO_SERVER);
        return 1;
    }

    int req_fd = open(CLIENT_TO_SERVER, O_RDONLY);
    int resp_fd = open(SERVER_TO_CLIENT, O_WRONLY);
    if (req_fd < 0 || resp_fd < 0) {
        perror("open");
        unlink(CLIENT_TO_SERVER);
        unlink(SERVER_TO_CLIENT);
        return 1;
    }

    printf("Server ready. Waiting for client...\n");
    char buffer[256];
    while (1) {
        ssize_t n = read(req_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("Server received: %s\n", buffer);

        char response[256];
        if (strcmp(buffer, "exit") == 0) {
            memcpy(response, "Bye from server", 15);
            response[15] = '\0';
            write(resp_fd, response, strlen(response) + 1);
            break;
        }

        memcpy(response, "ACK: ", 5);
        response[5] = '\0';
        strncat(response, buffer, sizeof(response) - strlen(response) - 1);
        write(resp_fd, response, strlen(response) + 1);
    }

    close(req_fd);
    close(resp_fd);
    unlink(CLIENT_TO_SERVER);
    unlink(SERVER_TO_CLIENT);
    return 0;
}

/*
 * Actual output of 38a.c
 * Command: gcc 38a.c -o 38a && ./38a &
 *
 * Server ready. Waiting for client...
 * Server received: Hello server
 * Server received: How are you?
 * Server received: exit
 */

