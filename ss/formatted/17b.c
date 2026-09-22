/*
17. Online Ticket Reservation Simulation: Develop a program to simulate online ticket reservation with the implementation of a write lock. Write one program to open a file, store a ticket number, and exit. Write a separate program to open the file, implement a write lock, read the ticket number, increment the number, print the new ticket number, and then close the file.
*/

// Program 2: Reserve ticket with write lock (ticket_reserve.c)
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>  // for flock

int main() {
    int fd = open("ticket.dat", O_RDWR);
    if (fd < 0) {
        perror("open failed (run init program first)");
        exit(1);
    }
    
    // Acquire write lock (exclusive lock)
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  // Lock entire file
    
    printf("Waiting for write lock...\n");
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl write lock failed");
        close(fd);
        exit(1);
    }
    
    printf("Write lock acquired\n");
    
    // Read current ticket number
    int ticket;
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &ticket, sizeof(int)) != sizeof(int)) {
        perror("read failed");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        exit(1);
    }
    
    printf("Current ticket number: %d\n", ticket);
    
    // Increment ticket number
    ticket++;
    printf("New ticket number: %d\n", ticket);
    
    // Write back
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &ticket, sizeof(int)) != sizeof(int)) {
        perror("write failed");
    }
    
    // Release lock
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl unlock failed");
    } else {
        printf("Write lock released\n");
    }
    
    close(fd);
    return 0;
}

/*
 * Sample output for 17b.c
 * Example runtime:
 * $ gcc 17b.c -o 17b
 * $ ./17b
 *
 * Write lock acquired on record 1
 */

/*
 * Actual output of 17b.c
 * Command: gcc 17b.c -o 17b
 *
 * open failed (run init program first): No such file or directory
 */

