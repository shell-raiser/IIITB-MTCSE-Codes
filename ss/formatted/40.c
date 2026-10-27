/*
40. Process File Limit and Pipe Size: Write a program to print the maximum number of files that can be opened within a process and the size of a pipe (circular buffer).
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

int main(void) {
    /* 1. Maximum files a process may open: RLIMIT_NOFILE */
    printf("Maximum number of open files per process:\n");

    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        if (rlim.rlim_cur == RLIM_INFINITY)
            printf("  Soft limit : unlimited\n");
        else
            printf("  Soft limit : %lu\n", (unsigned long)rlim.rlim_cur);

        if (rlim.rlim_max == RLIM_INFINITY)
            printf("  Hard limit : unlimited\n");
        else
            printf("  Hard limit : %lu\n", (unsigned long)rlim.rlim_max);
    } else {
        perror("getrlimit(RLIMIT_NOFILE)");
    }

    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max > 0)
        printf("  sysconf(_SC_OPEN_MAX) : %ld\n", open_max);
    else
        printf("  sysconf(_SC_OPEN_MAX) : unavailable\n");

    /* 2. Pipe (circular buffer) size */
    printf("\nPipe (circular buffer) size:\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    long pipe_size = fcntl(pipefd[0], F_GETPIPE_SZ);
    if (pipe_size != -1)
        printf("  Current capacity (F_GETPIPE_SZ) : %ld bytes\n", pipe_size);
    else
        perror("fcntl(F_GETPIPE_SZ)");

    long pipe_buf = fpathconf(pipefd[0], _PC_PIPE_BUF);
    if (pipe_buf != -1)
        printf("  Atomic write size (PIPE_BUF)   : %ld bytes\n", pipe_buf);
    else
        perror("fpathconf(_PC_PIPE_BUF)");

    printf("  PIPE_BUF (limits.h)            : %d bytes\n", PIPE_BUF);

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
/*
 * Output
 * Command: gcc 40.c -o 40 && ./40
 *
Maximum number of open files per process:
  Soft limit : 1048576
  Hard limit : 1048576
  sysconf(_SC_OPEN_MAX) : 1048576

Pipe (circular buffer) size:
  Current capacity (F_GETPIPE_SZ) : 65536 bytes
  Atomic write size (PIPE_BUF)   : 4096 bytes
  PIPE_BUF (limits.h)            : 4096 bytes

 */
