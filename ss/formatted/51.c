/*
51. Inter-Machine Communication using Socket: Write a program to communicate between two machines using sockets.
*/

// Server program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void run_server() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(1);
    }
    
    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(1);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // Accept connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(1);
    }
    
    printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    
    // Communication loop
    while (1) {
        int valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread <= 0) break;
        
        buffer[valread] = '\0';
        printf("Received: %s\n", buffer);
        
        if (strcmp(buffer, "exit") == 0) break;
        
        char *response = "Message received by server";
        send(client_fd, response, strlen(response), 0);
    }
    
    close(client_fd);
    close(server_fd);
}

// Client program
void run_client(char *server_ip) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(1);
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(1);
    }
    
    printf("Connected to server %s:%d\n", server_ip, PORT);
    
    char *messages[] = {"Hello Server", "How are you?", "Goodbye", "exit", NULL};
    
    for (int i = 0; messages[i] != NULL; i++) {
        send(sock, messages[i], strlen(messages[i]), 0);
        printf("Sent: %s\n", messages[i]);
        
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("Server response: %s\n", buffer);
        }
        sleep(1);
    }
    
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <server|client> [server_ip]\n", argv[0]);
        printf("  Server: %s server\n", argv[0]);
        printf("  Client: %s client <server_ip>\n", argv[0]);
        exit(1);
    }
    
    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        if (argc < 3) {
            printf("Client needs server IP\n");
            exit(1);
        }
        run_client(argv[2]);
    } else {
        printf("Invalid mode\n");
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 51.c -o 51
 *
 * Usage: /tmp/ss_run_51_48157 <server|client> [server_ip]
 *   Server: /tmp/ss_run_51_48157 server
 *   Client: /tmp/ss_run_51_48157 client <server_ip>
 */

