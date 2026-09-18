// 10 File Write and Seek: Implement a program to open a file in read-write mode, write
// 10 bytes, move the file pointer by 10 bytes using lseek, and then write another 10
// bytes.
// a. Check the return value of lseek.
// b. Open the file with od command and examine the empty spaces between the data.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    const char *filename = "seek_test.txt";
    
    // Open file in read-write mode, create if not exists, truncate
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("File opened with fd: %d\n", fd);

    // Write first 10 bytes
    const char *data1 = "AAAAAAAAAA";  // 10 bytes
    ssize_t written = write(fd, data1, 10);
    printf("Wrote %zd bytes: %s\n", written, data1);

    // Get current position
    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Current position after first write: %ld\n", pos);

    // Move file pointer by 10 bytes from current position
    off_t new_pos = lseek(fd, 10, SEEK_CUR);
    if (new_pos == (off_t)-1) {
        perror("lseek");
    } else {
        printf("lseek returned: %ld (moved 10 bytes forward from current)\n", new_pos);
    }

    // Write another 10 bytes
    const char *data2 = "BBBBBBBBBB";  // 10 bytes
    written = write(fd, data2, 10);
    printf("Wrote %zd bytes: %s\n", written, data2);

    // Get final position
    pos = lseek(fd, 0, SEEK_CUR);
    printf("Final position: %ld\n", pos);

    // Close file
    close(fd);
    printf("\nFile closed. Now examine with od command:\n");
    printf("Run: od -c %s\n", filename);
    printf("Run: od -x %s\n", filename);
    printf("Run: xxd %s\n", filename);

    return 0;
}

/* Commands to run:
   gcc "10 File Write and Seek.c" -o 10_file_seek
   ./10_file_seek
   od -c seek_test.txt
   od -x seek_test.txt
   xxd seek_test.txt
   ls -l seek_test.txt
   
   Expected output from od -c:
   0000000    A   A   A   A   A   A   A   A   A   A  \0  \0  \0  \0  \0  \0
   0000020   \0  \0  \0  \0   B   B   B   B   B   B   B   B   B   B
   0000036
   
   The 10 null bytes (\0) are the "hole" created by lseek.
   File size will be 30 bytes but only 20 bytes of actual data.
*/