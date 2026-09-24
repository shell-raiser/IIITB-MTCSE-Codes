/*
17. Online Ticket Reservation Simulation: Develop a program to simulate online ticket reservation with the implementation of a write lock. Write one program to open a file, store a ticket number, and exit. Write a separate program to open the file, implement a write lock, read the ticket number, increment the number, print the new ticket number, and then close the file.
*/

// Program 1: Initialize ticket file (ticket_init.c)
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>  // for flock

int main() {
    int fd = open("ticket.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    // lock and initialize ticket number
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  // Lock entire file

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl write lock failed");
        close(fd);
        exit(1);
    }

    int ticket = 1000;
    if (write(fd, &ticket, sizeof(int)) != sizeof(int)) {
        perror("write failed");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        exit(1);
    }

    printf("Ticket file initialized with ticket number: %d\n", ticket);
    sleep(10);  
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 0;
}
/*
 * Output
 * Command: gcc 17a.c -o 17a
 *
 * Ticket file initialized with ticket number: 1000
 */