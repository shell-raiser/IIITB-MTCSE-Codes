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

void test_dup2(const char *filename, const char *label) {
    printf("\n=== %s ===\n", label);
    
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }
    
    printf("Original fd: %d\n", fd);
    
    // b. Use dup2 - duplicate to specific fd (e.g., 10)
    int fd_dup2 = dup2(fd, 10);
    if (fd_dup2 < 0) {
        perror("dup2");
        close(fd);
        return;
    }
    printf("Duplicated fd (dup2 to 10): %d\n", fd_dup2);
    
    const char *msg1 = "Written by original fd\n";
    const char *msg2 = "Written by dup2 fd\n";
    
    write(fd, msg1, strlen(msg1));
    write(fd_dup2, msg2, strlen(msg2));
    
    close(fd);
    close(fd_dup2);
    
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
    test_dup2("test_dup2.txt", "Test b: dup2()");    
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
 * Command: gcc 11b.c -o 11b
 *
  * === Test b: dup2() ===
 * Original fd: 3
 * Duplicated fd (dup2 to 10): 10
 * File contents:
 * Written by original fd
 * Written by dup2 fd
 */

