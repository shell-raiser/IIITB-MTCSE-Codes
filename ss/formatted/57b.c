/*
57. System Limitation Exploration
 b. Maximum number of simultaneous processes per user ID.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main(void) {
    long child_max = sysconf(_SC_CHILD_MAX);
    if (child_max == -1)
        perror("sysconf(_SC_CHILD_MAX)");
    else
        printf("b. CHILD_MAX (max simultaneous processes per UID): %ld\n", child_max);

    struct rlimit rlim;
    if (getrlimit(RLIMIT_NPROC, &rlim) == 0) {
        printf("   RLIMIT_NPROC soft: ");
        if (rlim.rlim_cur == RLIM_INFINITY) printf("unlimited\n");
        else printf("%ld\n", (long)rlim.rlim_cur);

        printf("   RLIMIT_NPROC hard: ");
        if (rlim.rlim_max == RLIM_INFINITY) printf("unlimited\n");
        else printf("%ld\n", (long)rlim.rlim_max);
    } else {
        perror("getrlimit(RLIMIT_NPROC)");
    }

    return 0;
}
/*
 * Output
 * Command: gcc 57b.c -o 57b && ./57b
 *
 * b. CHILD_MAX (max simultaneous processes per UID): 31324
 *    RLIMIT_NPROC soft: 31324
 *    RLIMIT_NPROC hard: 31324
 */
