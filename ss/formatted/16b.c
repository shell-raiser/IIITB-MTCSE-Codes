// 16.	Mandatory Locking Implementation: Write a program to perform mandatory locking with the following implementations:
// a. Implement a write lock.
// b. Implement a read lock.
// https://unix.stackexchange.com/questions/147392/what-is-advisory-locking-on-files-that-unix-systems-typically-employs
    // Advisory locking is for processes that cooperate "peacefully". The kernel keeps track of the locks but doesn't enforce them - it's up to the applications to obey them. This way the kernel doesn't need to deal with situations like dead-locks.
    // Mandatory locking suspends a process until the requested file segments are free. Advisory locking returns a result indicating whether the lock was obtained or not: processes can ignore the result and do the I/O anyway. You cannot use both mandatory and advisory file locking on the same file at the same time. The mode of a file at the time it is opened determines whether locks on a file are treated as mandatory or advisory. https://docs.oracle.com/cd/E19455-01/806-4750/fileio-11/index.html




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
    const char *path = "mandatory_lock_read.txt";
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (write(fd, "initial data\n", 13) < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl F_SETLK");
        close(fd);
        return 1;
    }

    printf("Read lock acquired on %s\n", path);

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl unlock");
    }

    close(fd);
    return 0;
}

/*
 * Sample output for 16b.c
 * Example runtime:
 * $ gcc 16b.c -o 16b
 * $ ./16b
 *
 * Mandatory locking example: read lock requested
 */

/*
 * Actual output of 16b.c
 * Command: gcc 16b.c -o 16b
 *
 * ./16b.c: In function ‘main’:
 * ./16b.c:23:5: error: ‘flock’ undeclared (first use in this function)
 *    23 |     flock.l_type =
 *       |     ^~~~~
 * ./16b.c:23:5: note: each undeclared identifier is reported only once for each function it appears in
 * ./16b.c:24:5: error: expected expression before ‘int’
 *    24 |     int fd = open(“temp”, O_RDWR, 0744);
 *       |     ^~~
 * ./16b.c:24:19: error: stray ‘\342’ in program
 *    24 |     int fd = open(<U+201C>temp<U+201D>, O_RDWR, 0744);
 *       |                   ^~~~~~~~
 * ./16b.c:24:24: error: stray ‘\342’ in program
 *    24 |     int fd = open(<U+201C>temp<U+201D>, O_RDWR, 0744);
 *       |                               ^~~~~~~~
 * ./16b.c:25:5: error: implicit declaration of function ‘fcntl’ [-Wimplicit-function-declaration]
 *    25 |     fcntl (int fd, int cmd, struct flock &);
 *       |     ^~~~~
 * ./16b.c:25:12: error: expected expression before ‘int’
 *    25 |     fcntl (int fd, int cmd, struct flock &);
 *       |            ^~~
 */

