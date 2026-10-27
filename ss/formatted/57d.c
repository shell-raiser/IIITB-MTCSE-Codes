/*
57. System Limitation Exploration
 d. Maximum number of open files.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main(void) {
    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max == -1)
        perror("sysconf(_SC_OPEN_MAX)");
    else
        printf("d. OPEN_MAX (max open files per process): %ld\n", open_max);

    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("   RLIMIT_NOFILE soft: ");
        if (rlim.rlim_cur == RLIM_INFINITY) printf("unlimited\n");
        else printf("%ld\n", (long)rlim.rlim_cur);

        printf("   RLIMIT_NOFILE hard: ");
        if (rlim.rlim_max == RLIM_INFINITY) printf("unlimited\n");
        else printf("%ld\n", (long)rlim.rlim_max);
    } else {
        perror("getrlimit(RLIMIT_NOFILE)");
    }

    return 0;
}
/*
 * Output
 * Command: gcc 57d.c -o 57d && ./57d
 *
 * d. OPEN_MAX (max open files per process): 1048576
 *    RLIMIT_NOFILE soft: 1048576
 *    RLIMIT_NOFILE hard: 1048576
 */
