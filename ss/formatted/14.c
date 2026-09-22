// 14. File Type Identification: Write a program that takes input from the command line
// and identifies the type of file. Ensure the program can recognize various file types.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/sysmacros.h>

void print_file_type(const char *path) {
    struct stat sb;
    
    // Use lstat to not follow symbolic links
    if (lstat(path, &sb) < 0) {
        perror("lstat");
        return;
    }
    
    printf("File: %s\n", path);
    printf("  Inode: %lu\n", sb.st_ino);
    printf("  Permissions: %o\n", sb.st_mode & 0777);
    printf("  Type: ");
    
    switch (sb.st_mode & S_IFMT) {
        case S_IFREG:
            printf("Regular file");
            break;
        case S_IFDIR:
            printf("Directory");
            break;
        case S_IFLNK:
            printf("Symbolic link");
            // Read the link target
            char link_target[256];
            ssize_t len = readlink(path, link_target, sizeof(link_target) - 1);
            if (len >= 0) {
                link_target[len] = '\0';
                printf(" -> %s", link_target);
            }
            break;
        case S_IFIFO:
            printf("FIFO / Named pipe");
            break;
        case S_IFSOCK:
            printf("Socket");
            break;
        case S_IFCHR:
            printf("Character device");
            printf(" (major: %d, minor: %d)", major(sb.st_rdev), minor(sb.st_rdev));
            break;
        case S_IFBLK:
            printf("Block device");
            printf(" (major: %d, minor: %d)", major(sb.st_rdev), minor(sb.st_rdev));
            break;
        default:
            printf("Unknown");
            break;
    }
    printf("\n");
    printf("  Size: %ld bytes\n", sb.st_size);
    printf("  Links: %lu\n", sb.st_nlink);
    printf("  UID: %u, GID: %u\n", sb.st_uid, sb.st_gid);
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    
    printf("File Type Identification\n");
    printf("========================\n\n");
    
    for (int i = 1; i < argc; i++) {
        print_file_type(argv[i]);
    }
    
    return 0;
}

/* Commands to run:
   gcc "14 File Type Identification.c" -o 14_file_type
   ./14_file_type /etc/passwd /dev/null /tmp /bin/ls
   
   # Create test files first
   touch regular_file.txt
   mkdir test_dir
   ln -s regular_file.txt symlink.txt
   mkfifo my_fifo
   ./14_file_type regular_file.txt test_dir symlink.txt my_fifo /dev/tty /dev/sda
*/
/*
 * Output
 * Command: gcc 14.c -o 14
 *
 * Usage: /tmp/ss_run_14_48157 <file1> [file2] ...
 */

