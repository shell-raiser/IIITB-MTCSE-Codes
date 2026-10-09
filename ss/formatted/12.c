// 12. Determining Opening Mode of a File: Create a program to find out the opening
// mode of a file using the fcntl system call.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void print_access_mode(int flags) {
    int access_mode = flags & O_ACCMODE;
    printf("Access mode: ");
    switch (access_mode) {
        case O_RDONLY:  printf("O_RDONLY (read only)"); break;
        case O_WRONLY:  printf("O_WRONLY (write only)"); break;
        case O_RDWR:    printf("O_RDWR (read/write)"); break;
        default:        printf("Unknown"); break;
    }
    printf("\n");
}

void print_status_flags(int flags) {
    printf("Status flags:\n");
    if (flags & O_APPEND)      printf("  O_APPEND\n");
    if (flags & O_NONBLOCK)    printf("  O_NONBLOCK\n");
    if (flags & O_SYNC)        printf("  O_SYNC\n");
    if (flags & O_DSYNC)       printf("  O_DSYNC\n");
    if (flags & O_ASYNC)       printf("  O_ASYNC\n");
#ifdef O_DIRECT
    if (flags & O_DIRECT)      printf("  O_DIRECT\n");
#endif
#ifdef O_NOATIME
    if (flags & O_NOATIME)     printf("  O_NOATIME\n");
#endif
#ifdef O_CLOEXEC
    if (flags & O_CLOEXEC)     printf("  O_CLOEXEC\n");
#endif
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Open file with various modes to test
    printf("=== Testing different open modes ===\n\n");

    // Test 1: Read-only
    int fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL);
        if (flags >= 0) {
            printf("File opened with O_RDONLY:\n");
            print_access_mode(flags);
            print_status_flags(flags);
            printf("Raw flags: 0x%x\n\n", flags);
        }
        close(fd);
    }

    // Test 2: Write-only
    fd = open(filename, O_WRONLY);
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL);
        if (flags >= 0) {
            printf("File opened with O_WRONLY:\n");
            print_access_mode(flags);
            print_status_flags(flags);
            printf("Raw flags: 0x%x\n\n", flags);
        }
        close(fd);
    }

    // Test 3: Read-write
    fd = open(filename, O_RDWR);
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL);
        if (flags >= 0) {
            printf("File opened with O_RDWR:\n");
            print_access_mode(flags);
            print_status_flags(flags);
            printf("Raw flags: 0x%x\n\n", flags);
        }
        close(fd);
    }

    // Test 4: Read-write with append
    fd = open(filename, O_RDWR | O_APPEND);
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL);
        if (flags >= 0) {
            printf("File opened with O_RDWR | O_APPEND:\n");
            print_access_mode(flags);
            print_status_flags(flags);
            printf("Raw flags: 0x%x\n\n", flags);
        }
        close(fd);
    }

    // Test 5: Read-write with non-blocking
    fd = open(filename, O_RDWR | O_NONBLOCK);
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL);
        if (flags >= 0) {
            printf("File opened with O_RDWR | O_NONBLOCK:\n");
            print_access_mode(flags);
            print_status_flags(flags);
            printf("Raw flags: 0x%x\n\n", flags);
        }
        close(fd);
    }

    // Demonstrate F_GETFD (file descriptor flags)
    fd = open(filename, O_RDWR);
    if (fd >= 0) {
        int fd_flags = fcntl(fd, F_GETFD);
        if (fd_flags >= 0) {
            printf("File descriptor flags (F_GETFD):\n");
            if (fd_flags & FD_CLOEXEC) {
                printf("  FD_CLOEXEC: Set (close on exec)\n");
            } else {
                printf("  FD_CLOEXEC: Not set\n");
            }
            printf("Raw fd flags: 0x%x\n", fd_flags);
        }
        close(fd);
    }

    return 0;
}

/* Commands to run:
   gcc "12 Determining Opening Mode of a File.c" -o 12_open_mode
   echo "test content" > testfile.txt
   ./12_open_mode testfile.txt
*/
/*
 * Output
 * Command: gcc 12.c -o 12
 *
 * Usage: /tmp/ss_run_12_48157 <filename>
 */

