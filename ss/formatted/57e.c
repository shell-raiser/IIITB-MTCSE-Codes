/*
57. System Limitation Exploration
 e. Size of a page.
*/

#include <stdio.h>
#include <unistd.h>

int main(void) {
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        perror("sysconf(_SC_PAGESIZE)");
        return 1;
    }

    printf("e. PAGE_SIZE: %ld bytes (%ld KB)\n", page_size, page_size / 1024);
    printf("   _SC_PAGE_SIZE synonym: %ld\n", sysconf(_SC_PAGE_SIZE));

    return 0;
}
/*
 * Output
 * Command: gcc 57e.c -o 57e && ./57e
 *
 * e. PAGE_SIZE: 4096 bytes (4 KB)
 *    _SC_PAGE_SIZE synonym: 4096
 */
