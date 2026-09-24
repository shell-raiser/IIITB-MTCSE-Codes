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


void test_fcntl(const char *filename, const char *label) {
    printf("\n=== %s ===\n", label);
    
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }
    
    printf("Original fd: %d\n", fd);
    
    // c. Use fcntl with F_DUPFD
    int fd_fcntl = fcntl(fd, F_DUPFD, 0);
    if (fd_fcntl < 0) {
        perror("fcntl F_DUPFD");
        close(fd);
        return;
    }
    printf("Duplicated fd (fcntl F_DUPFD): %d\n", fd_fcntl);
    
    // Also test F_DUPFD_CLOEXEC
    int fd_fcntl_cloexec = fcntl(fd, F_DUPFD_CLOEXEC, 0);
    if (fd_fcntl_cloexec >= 0) {
        printf("Duplicated fd (fcntl F_DUPFD_CLOEXEC): %d\n", fd_fcntl_cloexec);
    }
    
    const char *msg1 = "Written by original fd\n";
    const char *msg2 = "Written by fcntl fd\n";
    
    write(fd, msg1, strlen(msg1));
    write(fd_fcntl, msg2, strlen(msg2));
    
    close(fd);
    close(fd_fcntl);
    if (fd_fcntl_cloexec >= 0) close(fd_fcntl_cloexec);
    
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
    test_fcntl("test_fcntl.txt", "Test c: fcntl()");    
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
 * Command: gcc 11c.c -o 11c
 *
 * === Test c: fcntl() ===
 * Original fd: 3
 * Duplicated fd (fcntl F_DUPFD): 4
 * Duplicated fd (fcntl F_DUPFD_CLOEXEC): 5
 * File contents:
 * Written by original fd
 * Written by fcntl fd
 */

