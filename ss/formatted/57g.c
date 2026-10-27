/*
57. System Limitation Exploration
 g. Number of currently available pages in physical memory.
*/

#include <stdio.h>
#include <unistd.h>

int main(void) {
    long avail_pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size   = sysconf(_SC_PAGESIZE);

    if (avail_pages == -1 || page_size == -1) {
        perror("sysconf");
        return 1;
    }

    printf("g. AVPHYS_PAGES (currently available physical pages): %ld\n", avail_pages);
    printf("   Page size                                         : %ld bytes\n", page_size);
    printf("   Available physical memory                         : %.2f MB\n",
           (double)avail_pages * page_size / (1024.0 * 1024.0));

    return 0;
}
/*
 * Output
 * Command: gcc 57g.c -o 57g && ./57g
 *
 * g. AVPHYS_PAGES (currently available physical pages): 5921340
 *    Page size                                         : 4096 bytes
 *    Available physical memory                         : 23130.23 MB
 */
