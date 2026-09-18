// 4 Opening Existing File: Write a program to open an existing file in read-write mode.
// Experiment with the O_EXCL flag.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main() {
    // First, create a test file
    int fd = open("testfile.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open for creation");
        return 1;
    }
    write(fd, "Initial content\n", 16);
    close(fd);
    printf("Created testfile.txt\n\n");

    // 1. Open existing file in read-write mode
    fd = open("testfile.txt", O_RDWR);
    if (fd < 0) {
        perror("open O_RDWR");
        return 1;
    }
    printf("Opened testfile.txt in O_RDWR mode, fd = %d\n", fd);
    
    // Read content
    char buf[100];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Content: %s", buf);
    }
    
    // Write more content
    lseek(fd, 0, SEEK_END);
    write(fd, "Appended content\n", 17);
    close(fd);
    printf("Appended to file and closed.\n\n");

    // 2. Experiment with O_EXCL flag
    // O_EXCL with O_CREAT: fails if file already exists
    printf("--- O_EXCL Experiment ---\n");
    fd = open("testfile.txt", O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd < 0) {
        printf("open with O_CREAT|O_EXCL on existing file failed (expected): %s\n", strerror(errno));
    } else {
        printf("Unexpectedly succeeded!\n");
        close(fd);
    }

    // 3. O_EXCL with new file (should succeed)
    fd = open("newfile.txt", O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd < 0) {
        perror("open newfile with O_EXCL");
    } else {
        printf("Created newfile.txt with O_CREAT|O_EXCL, fd = %d\n", fd);
        write(fd, "New file content\n", 17);
        close(fd);
    }

    // 4. O_EXCL without O_CREAT (undefined behavior, typically ignored)
    fd = open("testfile.txt", O_EXCL | O_RDWR);
    if (fd < 0) {
        perror("open with O_EXCL only");
    } else {
        printf("Opened with O_EXCL only (no O_CREAT), fd = %d\n", fd);
        close(fd);
    }

    return 0;
}

/* Commands to run:
   gcc "4 Opening Existing File: Write a program.c" -o 4_open_existing
   ./4_open_existing
   ls -l testfile.txt newfile.txt
   cat testfile.txt
   cat newfile.txt
*/