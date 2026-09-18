/*
17. Online Ticket Reservation Simulation: Develop a program to simulate online ticket reservation with the implementation of a write lock. Write one program to open a file, store a ticket number, and exit. Write a separate program to open the file, implement a write lock, read the ticket number, increment the number, print the new ticket number, and then close the file.
*/

// Program 1: Initialize ticket file (ticket_init.c)
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("ticket.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    
    int initial_ticket = 1000;
    if (write(fd, &initial_ticket, sizeof(int)) != sizeof(int)) {
        perror("write failed");
        close(fd);
        exit(1);
    }
    
    printf("Ticket file initialized with ticket number: %d\n", initial_ticket);
    close(fd);
    return 0;
}
