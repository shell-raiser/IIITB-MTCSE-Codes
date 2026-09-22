/*
54. System Resource Limits: Write a program to print system resource limits using the getrlimit system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

void print_limit(int resource, const char *name) {
    struct rlimit rlim;
    if (getrlimit(resource, &rlim) == 0) {
        printf("%-30s: ", name);
        if (rlim.rlim_cur == RLIM_INFINITY) {
            printf("unlimited");
        } else {
            printf("%ld", rlim.rlim_cur);
        }
        printf(" (soft) / ");
        if (rlim.rlim_max == RLIM_INFINITY) {
            printf("unlimited");
        } else {
            printf("%ld", rlim.rlim_max);
        }
        printf(" (hard)\n");
    } else {
        perror(name);
    }
}

int main() {
    printf("System Resource Limits:\n");
    printf("========================\n");
    
    print_limit(RLIMIT_CPU, "RLIMIT_CPU (CPU time)");
    print_limit(RLIMIT_FSIZE, "RLIMIT_FSIZE (file size)");
    print_limit(RLIMIT_DATA, "RLIMIT_DATA (data segment)");
    print_limit(RLIMIT_STACK, "RLIMIT_STACK (stack size)");
    print_limit(RLIMIT_CORE, "RLIMIT_CORE (core file size)");
    print_limit(RLIMIT_RSS, "RLIMIT_RSS (resident set size)");
    print_limit(RLIMIT_NOFILE, "RLIMIT_NOFILE (open files)");
    print_limit(RLIMIT_AS, "RLIMIT_AS (address space)");
    print_limit(RLIMIT_NPROC, "RLIMIT_NPROC (processes)");
    print_limit(RLIMIT_MEMLOCK, "RLIMIT_MEMLOCK (locked memory)");
    print_limit(RLIMIT_LOCKS, "RLIMIT_LOCKS (file locks)");
    print_limit(RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING (pending signals)");
    print_limit(RLIMIT_MSGQUEUE, "RLIMIT_MSGQUEUE (message queues)");
    print_limit(RLIMIT_NICE, "RLIMIT_NICE (nice priority)");
    print_limit(RLIMIT_RTPRIO, "RLIMIT_RTPRIO (real-time priority)");
    print_limit(RLIMIT_RTTIME, "RLIMIT_RTTIME (real-time CPU time)");
    
    return 0;
}
