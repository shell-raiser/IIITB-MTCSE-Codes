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
#include <string.h>

int main() {
    // Get file descriptor limit
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("File descriptor limits:\n");
        printf("  Soft limit: %ld\n", rlim.rlim_cur);
        printf("  Hard limit: %ld\n", rlim.rlim_max);
    } else {
        perror("getrlimit failed");
    }
    
    // Get OPEN_MAX from sysconf instead of the outdated OPEN_MAX macro.
    printf("\nOPEN_MAX (runtime via sysconf): %ld\n", sysconf(_SC_OPEN_MAX));
    
    // Test actual maximum by opening files
    printf("\nTesting actual maximum open files...\n");
    int fd;
    int count = 0;
    while ((fd = open("/dev/null", O_RDONLY)) != -1) {
        count++;
        if (count % 100 == 0) {
            printf("  Opened %d files...\n", count);
        }
    }
    printf("Maximum files opened: %d\n", count);
    printf("Error: %s\n", strerror(errno));
    
    // Close all
    for (int i = 0; i < count; i++) {
        close(3 + i); // FDs start from 3 (0,1,2 are stdin,stdout,stderr)
    }
    
    // Get pipe size
    printf("\nPipe buffer size:\n");
    int pipefd[2];
    if (pipe(pipefd) == 0) {
        // Try to get pipe size using fcntl
        long pipe_size = fcntl(pipefd[0], F_GETPIPE_SZ);
        if (pipe_size != -1) {
            printf("  Pipe size (fcntl F_GETPIPE_SZ): %ld bytes\n", pipe_size);
        } else {
            perror("  fcntl F_GETPIPE_SZ failed");
        }
        
        // Try to write to determine capacity
        char buf[4096];
        int written = 0;
        int flags = fcntl(pipefd[1], F_GETFL);
        fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
        
        while (write(pipefd[1], buf, sizeof(buf)) > 0) {
            written += sizeof(buf);
        }
        printf("  Pipe capacity (non-blocking write): %d bytes\n", written);
        
        close(pipefd[0]);
        close(pipefd[1]);
    }
    
    // PIPE_BUF from limits.h
    printf("  PIPE_BUF (atomic write guarantee): %d bytes\n", PIPE_BUF);
    
    return 0;
}
/*
 * Output
 * Command: gcc 40.c -o 40
 *
 * ./40.c: In function ‘main’:
 * ./40.c:25:47: error: ‘OPEN_MAX’ undeclared (first use in this function); did you mean ‘FOPEN_MAX’?
 *    25 |     printf("\nOPEN_MAX (compile-time): %d\n", OPEN_MAX);
 *       |                                               ^~~~~~~~
 *       |                                               FOPEN_MAX
 * ./40.c:25:47: note: each undeclared identifier is reported only once for each function it appears in
 * ./40.c:38:27: error: implicit declaration of function ‘strerror’ [-Wimplicit-function-declaration]
 *    38 |     printf("Error: %s\n", strerror(errno));
 *       |                           ^~~~~~~~
 * ./40.c:12:1: note: ‘strerror’ is defined in header ‘<string.h>’; this is probably fixable by adding ‘#include <string.h>’
 *    11 | #include <errno.h>
 *   +++ |+#include <string.h>
 *    12 | 
 * ./40.c:50:43: error: ‘F_GETPIPE_SZ’ undeclared (first use in this function)
 *    50 |         long pipe_size = fcntl(pipefd[0], F_GETPIPE_SZ);
 *       |                                           ^~~~~~~~~~~~
 */

