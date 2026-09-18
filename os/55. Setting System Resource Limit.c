/*
55. Setting System Resource Limit: Write a program to set a system resource limit using the setrlimit system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

void set_limit(int resource, const char *name, rlim_t soft, rlim_t hard) {
    struct rlimit rlim;
    rlim.rlim_cur = soft;
    rlim.rlim_max = hard;
    
    printf("Setting %s: soft=%ld, hard=%ld\n", name, soft, hard);
    
    if (setrlimit(resource, &rlim) == 0) {
        printf("  Success\n");
    } else {
        printf("  Failed: %s\n", strerror(errno));
        if (errno == EPERM) {
            printf("  (Need root privileges to increase hard limit)\n");
        }
    }
    
    // Verify
    if (getrlimit(resource, &rlim) == 0) {
        printf("  Verified: soft=%ld, hard=%ld\n", rlim.rlim_cur, rlim.rlim_max);
    }
}

int main() {
    printf("Setting System Resource Limits:\n");
    printf("================================\n");
    
    // Example: Limit CPU time to 1 second (soft) / 2 seconds (hard)
    set_limit(RLIMIT_CPU, "RLIMIT_CPU", 1, 2);
    
    // Example: Limit file size to 1MB
    set_limit(RLIMIT_FSIZE, "RLIMIT_FSIZE", 1024*1024, 2*1024*1024);
    
    // Example: Limit number of open files
    set_limit(RLIMIT_NOFILE, "RLIMIT_NOFILE", 64, 128);
    
    // Example: Limit number of processes
    set_limit(RLIMIT_NPROC, "RLIMIT_NPROC", 10, 20);
    
    // Example: Limit stack size to 1MB
    set_limit(RLIMIT_STACK, "RLIMIT_STACK", 1024*1024, 2*1024*1024);
    
    // Example: Limit core dump size to 0 (disable)
    set_limit(RLIMIT_CORE, "RLIMIT_CORE", 0, 0);
    
    // Test CPU limit by infinite loop
    printf("\nTesting CPU limit with infinite loop...\n");
    while (1) {
        // This should be killed after 1 second CPU time
    }
    
    return 0;
}
