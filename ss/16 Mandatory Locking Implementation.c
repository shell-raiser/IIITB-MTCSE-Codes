// 16.	Mandatory Locking Implementation: Write a program to perform mandatory locking with the following implementations:
// a. Implement a write lock.
// b. Implement a read lock.
// https://unix.stackexchange.com/questions/147392/what-is-advisory-locking-on-files-that-unix-systems-typically-employs
    // Advisory locking is for processes that cooperate "peacefully". The kernel keeps track of the locks but doesn't enforce them - it's up to the applications to obey them. This way the kernel doesn't need to deal with situations like dead-locks.
    // Mandatory locking suspends a process until the requested file segments are free. Advisory locking returns a result indicating whether the lock was obtained or not: processes can ignore the result and do the I/O anyway. You cannot use both mandatory and advisory file locking on the same file at the same time. The mode of a file at the time it is opened determines whether locks on a file are treated as mandatory or advisory. https://docs.oracle.com/cd/E19455-01/806-4750/fileio-11/index.html




#include <sys/types.h>
int main(int argc, char const *argv[])
{
    struct flock
    {
        short l_type;
        /* lock type: read, write or unlock */
        short l_whence;
        off_t l_start;
        off_t l_len;
        pid_t l_pid;
    };
    flock.l_type = 
    int fd = open(“temp”, O_RDWR, 0744);
    fcntl (int fd, int cmd, struct flock &);


    return 0;
}
