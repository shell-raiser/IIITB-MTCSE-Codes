/*
38. FIFO Communication (Two-Way): Write two programs to establish communication through FIFO using two-way communication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        unlink(CLIENT_TO_SERVER);
        unlink(SERVER_TO_CLIENT);
        return 1;
    }

    if (pid == 0) {
        int req_fd = open(CLIENT_TO_SERVER, O_WRONLY);
        int resp_fd = open(SERVER_TO_CLIENT, O_RDONLY);
        if (req_fd < 0 || resp_fd < 0) {
            perror("client open");
            _exit(1);
        }

        const char *messages[] = {"Hello from child", "exit", NULL};
        for (int i = 0; messages[i] != NULL; i++) {
            write(req_fd, messages[i], strlen(messages[i]) + 1);
            printf("Child sent: %s\n", messages[i]);

            char buffer[256];
            ssize_t n = read(resp_fd, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("Child received: %s\n", buffer);
            }
        }

        close(req_fd);
        close(resp_fd);
        _exit(0);
    }

    int req_fd = open(CLIENT_TO_SERVER, O_RDONLY);
    int resp_fd = open(SERVER_TO_CLIENT, O_WRONLY);
    if (req_fd < 0 || resp_fd < 0) {
        perror("parent open");
        waitpid(pid, NULL, 0);
        unlink(CLIENT_TO_SERVER);
        unlink(SERVER_TO_CLIENT);
        return 1;
    }

    printf("Parent server is running...\n");
    char buffer[256];
    while (1) {
        ssize_t n = read(req_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("Parent received: %s\n", buffer);

        char response[256];
        if (strcmp(buffer, "exit") == 0) {
            memcpy(response, "Server closing connection", 25);
            response[25] = '\0';
            write(resp_fd, response, strlen(response) + 1);
            break;
        }

        memcpy(response, "Server echo: ", 13);
        response[13] = '\0';
        strncat(response, buffer, sizeof(response) - strlen(response) - 1);
        write(resp_fd, response, strlen(response) + 1);
    }

    close(req_fd);
    close(resp_fd);
    waitpid(pid, NULL, 0);
    unlink(CLIENT_TO_SERVER);
    unlink(SERVER_TO_CLIENT);
    return 0;
}

/*
 * Actual output of 38c.c
 * Command: gcc 38c.c -o 38c && ./38c
 *
 * Parent server is running...
 * Child sent: Hello from child
 * Parent received: Hello from child
 * Child received: Server echo: Hello from child
 * Child sent: exit
 * Parent received: exit
 * Child received: Server closing connection
 */

