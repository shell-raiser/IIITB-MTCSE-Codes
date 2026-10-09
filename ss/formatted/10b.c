// 10. File Write and Seek: Implement a program to open a file in read-write mode, write
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

    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *data1 = "AAAAAAAAAA";
    ssize_t written = write(fd, data1, 10);
    printf("Wrote %zd bytes: %s\n", written, data1);

    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Current position after first write: %ld\n", pos);

    off_t new_pos = lseek(fd, 10, SEEK_CUR);
    if (new_pos == (off_t)-1) {
        perror("lseek");
    } else {
        printf("lseek returned: %ld (moved 10 bytes forward from current)\n", new_pos);
    }

    const char *data2 = "BBBBBBBBBB";
    written = write(fd, data2, 10);
    printf("Wrote %zd bytes: %s\n", written, data2);

    pos = lseek(fd, 0, SEEK_CUR);
    printf("Final position: %ld\n", pos);

    close(fd);
    printf("\nFile closed. Now examine with od command:\n");
    printf("Run: od -c %s\n", filename);
    printf("Run: od -x %s\n", filename);
    printf("Run: xxd %s\n", filename);

    return 0;
}

/*
 * Output
 * Command: gcc 10b.c -o 10b && ./10b
 *
 * Wrote 10 bytes: AAAAAAAAAA
 * Current position after first write: 10
 * lseek returned: 20 (moved 10 bytes forward from current)
 * Wrote 10 bytes: BBBBBBBBBB
 * Final position: 30
 *
 * File closed. Now examine with od command:
 * Run: od -c seek_test.txt
 * Run: od -x seek_test.txt
 * Run: xxd seek_test.txt
 */
