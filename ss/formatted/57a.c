/*
57. System Limitation Exploration
 a. Maximum length of arguments in the exec family of functions.
*/

#include <stdio.h>
#include <limits.h>
#include <unistd.h>

int main(void) {
    long arg_max = sysconf(_SC_ARG_MAX);
    if (arg_max == -1) {
        perror("sysconf(_SC_ARG_MAX)");
        return 1;
    }

    printf("a. ARG_MAX (max total size of argv + envp for exec): %ld bytes\n", arg_max);

    /* POSIX also guarantees at least _POSIX_ARG_MAX (4096). */
    printf("   POSIX minimum guarantee              : %d bytes\n", _POSIX_ARG_MAX);

    return 0;
}
/*
 * Output
 * Command: gcc 57a.c -o 57a && ./57a
 *
 * a. ARG_MAX (max total size of argv + envp for exec): 2097152 bytes
 *    POSIX minimum guarantee              : 4096 bytes
 */
