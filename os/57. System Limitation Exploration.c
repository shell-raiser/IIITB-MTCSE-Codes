/*
57. System Limitation Exploration: Write a program to print system limitations for:
a. Maximum length of arguments in the exec family of functions.
b. Maximum number of simultaneous processes per user ID.
c. Number of clock ticks (jiffies) per second.
d. Maximum number of open files.
e. Size of a page.
f. Total number of pages in physical memory.
g. Number of currently available pages in physical memory.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <linux/unistd.h>

int main() {
    printf("System Limitations:\n");
    printf("==================\n");
    
    // a. Maximum length of arguments in exec family
    long arg_max = sysconf(_SC_ARG_MAX);
    if (arg_max != -1) {
        printf("a. ARG_MAX (max exec arguments): %ld bytes\n", arg_max);
    } else {
        perror("sysconf _SC_ARG_MAX");
    }
    
    // b. Maximum number of simultaneous processes per user ID
    long child_max = sysconf(_SC_CHILD_MAX);
    if (child_max != -1) {
        printf("b. CHILD_MAX (max processes per user): %ld\n", child_max);
    } else {
        perror("sysconf _SC_CHILD_MAX");
    }
    
    // Also check RLIMIT_NPROC
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NPROC, &rlim) == 0) {
        printf("   RLIMIT_NPROC soft: %ld, hard: %ld\n", rlim.rlim_cur, rlim.rlim_max);
    }
    
    // c. Number of clock ticks (jiffies) per second
    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck != -1) {
        printf("c. CLK_TCK (clock ticks per second): %ld\n", clk_tck);
    } else {
        perror("sysconf _SC_CLK_TCK");
    }
    
    // d. Maximum number of open files
    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max != -1) {
        printf("d. OPEN_MAX (max open files): %ld\n", open_max);
    } else {
        perror("sysconf _SC_OPEN_MAX");
    }
    // Also check RLIMIT_NOFILE
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("   RLIMIT_NOFILE soft: %ld, hard: %ld\n", rlim.rlim_cur, rlim.rlim_max);
    }
    
    // e. Size of a page
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size != -1) {
        printf("e. PAGE_SIZE: %ld bytes (%ld KB)\n", page_size, page_size / 1024);
    } else {
        perror("sysconf _SC_PAGESIZE");
    }
    
    // f. Total number of pages in physical memory
    long phys_pages = sysconf(_SC_PHYS_PAGES);
    if (phys_pages != -1) {
        printf("f. PHYS_PAGES (total physical pages): %ld\n", phys_pages);
        printf("   Total physical memory: %.2f MB\n", (double)phys_pages * page_size / (1024*1024));
    } else {
        perror("sysconf _SC_PHYS_PAGES");
    }
    
    // g. Number of currently available pages in physical memory
    long avail_pages = sysconf(_SC_AVPHYS_PAGES);
    if (avail_pages != -1) {
        printf("g. AVPHYS_PAGES (available physical pages): %ld\n", avail_pages);
        printf("   Available memory: %.2f MB\n", (double)avail_pages * page_size / (1024*1024));
    } else {
        perror("sysconf _SC_AVPHYS_PAGES");
    }
    
    // Additional useful limits
    printf("\nAdditional limits:\n");
    printf("  LOGIN_NAME_MAX: %ld\n", sysconf(_SC_LOGIN_NAME_MAX));
    printf("  HOST_NAME_MAX: %ld\n", sysconf(_SC_HOST_NAME_MAX));
    printf("  PATH_MAX: %ld\n", sysconf(_SC_PATH_MAX));
    printf("  PIPE_BUF: %ld\n", sysconf(_SC_PIPE_BUF));
    printf("  STREAM_MAX: %ld\n", sysconf(_SC_STREAM_MAX));
    printf("  TZNAME_MAX: %ld\n", sysconf(_SC_TZNAME_MAX));
    
    return 0;
}
