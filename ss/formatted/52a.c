/*
52. Concurrent Server Creation: Write a program to create a concurrent server:
a. Use fork
b. Use pthread_create
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// a. Fork-based concurrent server
void handle_client_fork(int client_fd) {
    char buffer[BUFFER_SIZE];
    int valread;
    
    printf("Child %d: Handling client\n", getpid());
    
    while ((valread = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        printf("Child %d: Received: %s\n", getpid(), buffer);
        
        if (strcmp(buffer, "exit") == 0) break;
        
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Echo from child %d: %.256s", getpid(), buffer);
        write(client_fd, response, strlen(response));
    }
    
    close(client_fd);
    printf("Child %d: Client disconnected\n", getpid());
    exit(0);
}


int main(int argc, char *argv[]) {
 
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);
    
    printf("Fork server listening on port %d...\n", PORT);
    
    // Handle SIGCHLD to avoid zombies
    signal(SIGCHLD, SIG_IGN);
    
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) continue;
        
        printf("New connection from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client_fork(client_fd);
        } else {
            close(client_fd);
        }
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 52a.c -o 52a
 *
 * Usage: /tmp/ss_run_52a_48157 <fork|thread>
 */

