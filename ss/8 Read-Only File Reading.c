// 8 Read-Only File Reading: Write a program to open a file in read-only mode, read
// line by line, and display each line as it is read. Close the file upon reaching the end of
// the file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 1024

// Function to read a line from file descriptor
// Returns: number of chars read (excluding newline), 0 at EOF, -1 on error
ssize_t read_line(int fd, char *buf, size_t bufsize) {
    size_t i = 0;
    char c;
    ssize_t n;
    
    while (i < bufsize - 1) {
        n = read(fd, &c, 1);
        if (n < 0) {
            return -1;  // Error
        }
        if (n == 0) {
            if (i == 0) return 0;  // EOF
            break;  // EOF with partial line
        }
        buf[i++] = c;
        if (c == '\n') break;
    }
    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Open file in read-only mode
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Reading file: %s (fd=%d)\n\n", filename, fd);

    char line[BUF_SIZE];
    ssize_t len;
    int line_num = 0;

    // Read line by line
    while ((len = read_line(fd, line, BUF_SIZE)) > 0) {
        line_num++;
        printf("Line %d: %s", line_num, line);
    }

    if (len < 0) {
        perror("read_line");
    } else {
        printf("\nEnd of file reached. Total lines: %d\n", line_num);
    }

    // Close the file
    close(fd);
    printf("File closed.\n");

    return 0;
}

/* Commands to run:
   gcc "8 Read-Only File Reading.c" -o 8_readonly_read
   echo -e "Line 1\nLine 2\nLine 3\nLine 4\nLine 5" > testfile.txt
   ./8_readonly_read testfile.txt
*/