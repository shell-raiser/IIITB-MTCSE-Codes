// 7 File Copying: Develop a program to copy the contents of file1 into file2, emulating
// the behavior of the $cp file1 file2 command.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <dest_file>\n", argv[0]);
        return 1;
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];

    // Open source file for reading
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        perror("open source file");
        return 1;
    }

    // Get source file permissions
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) < 0) {
        perror("fstat");
        close(src_fd);
        return 1;
    }

    // Open destination file for writing (create if not exists, truncate if exists)
    int dst_fd = open(dst_path, O_CREAT | O_WRONLY | O_TRUNC, src_stat.st_mode & 0777);
    if (dst_fd < 0) {
        perror("open destination file");
        close(src_fd);
        return 1;
    }

    // Copy data
    char buffer[BUF_SIZE];
    ssize_t bytes_read, bytes_written;
    
    while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0) {
        char *buf_ptr = buffer;
        ssize_t remaining = bytes_read;
        
        while (remaining > 0) {
            bytes_written = write(dst_fd, buf_ptr, remaining);
            if (bytes_written < 0) {
                if (errno == EINTR) continue;
                perror("write");
                close(src_fd);
                close(dst_fd);
                return 1;
            }
            remaining -= bytes_written;
            buf_ptr += bytes_written;
        }
    }

    if (bytes_read < 0) {
        perror("read");
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    // Close files
    close(src_fd);
    close(dst_fd);

    printf("File copied successfully: %s -> %s\n", src_path, dst_path);
    
    // Verify by showing both files
    printf("\nVerification:\n");
    system("ls -l \"$1\" \"$2\"");
    
    return 0;
}

/* Commands to run:
   gcc "7 File Copying.c" -o 7_file_copy
   echo "Hello World\nThis is a test file.\nLine 3" > source.txt
   ./7_file_copy source.txt destination.txt
   cat destination.txt
   diff source.txt destination.txt
   ls -l source.txt destination.txt
*/