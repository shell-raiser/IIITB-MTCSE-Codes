// 11. File Descriptor Duplication and Appending: Write a program to open a file,
// duplicate the file descriptor, append the file with both descriptors, and verify
// whether the file is updated correctly.
// a. Use dup
// b. Use dup2
// c. Use fcntl

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void test_dup(const char *filename, const char *label) {
    printf("\n=== %s ===\n", label);
    
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }
    
    printf("Original fd: %d\n", fd);
    
    // a. Use dup
    int fd_dup = dup(fd);
    if (fd_dup < 0) {
        perror("dup");
        close(fd);
        return;
    }
    printf("Duplicated fd (dup): %d\n", fd_dup);
    
    // Write using both descriptors
    const char *msg1 = "Written by original fd\n";
    const char *msg2 = "Written by dup fd\n";
    
    write(fd, msg1, strlen(msg1));
    write(fd_dup, msg2, strlen(msg2));
    
    close(fd);
    close(fd_dup);
    
    // Verify
    FILE *fp = fopen(filename, "r");
    if (fp) {
        char ch;
        printf("File contents:\n");
        while ((ch = fgetc(fp)) != EOF) putchar(ch);
        fclose(fp);
    }
}

int main() {
    test_dup("test_dup.txt", "Test a: dup()");
    
    return 0;
}

/* Commands to run:
   gcc "11 File Descriptor Duplication and Appending.c" -o 11_fd_dup
   ./11_fd_dup
   cat test_dup.txt
   cat test_dup2.txt
   cat test_fcntl.txt
*/
/*
 * Output
 * Command: gcc 11a.c -o 11a
 *
 * === Test a: dup() ===
 * Original fd: 3
 * Duplicated fd (dup): 4
 * File contents:
 * Written by original fd
 * Written by dup fd
 *
 */

