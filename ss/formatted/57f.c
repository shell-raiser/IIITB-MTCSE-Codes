/*
57. System Limitation Exploration
 f. Total number of pages in physical memory.
*/

#include <stdio.h>
#include <unistd.h>

int main(void) {
    long phys_pages = sysconf(_SC_PHYS_PAGES);
    long page_size  = sysconf(_SC_PAGESIZE);

    if (phys_pages == -1 || page_size == -1) {
        perror("sysconf");
        return 1;
    }

    printf("f. PHYS_PAGES (total physical pages): %ld\n", phys_pages);
    printf("   Page size                          : %ld bytes\n", page_size);
    printf("   Total physical memory              : %.2f MB (%.2f GB)\n",
           (double)phys_pages * page_size / (1024.0 * 1024.0),
           (double)phys_pages * page_size / (1024.0 * 1024.0 * 1024.0));

    return 0;
}
/*
 * Output
 * Command: gcc 57f.c -o 57f && ./57f
 *
 * f. PHYS_PAGES (total physical pages): 8154163
 *    Page size                          : 4096 bytes
 *    Total physical memory              : 31852.20 MB (31.11 GB)
 */
