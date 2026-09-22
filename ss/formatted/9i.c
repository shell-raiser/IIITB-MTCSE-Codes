// 9. File Information Extraction: Create a program to print various details about a given
// file, including:
// a. Inode
// b. Number of hard links
// c. UID
// d. GID
// e. Size
// f. Block size
// g. Number of blocks
// h. Time of last access
// i. Time of last modification
// j. Time of last change

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

void print_time(const char *label, time_t t) {
    char buf[64];
    struct tm *tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("%-25s: %s\n", label, buf);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    struct stat sb;

    // Use stat to get file information
    if (stat(filename, &sb) < 0) {
        perror("stat");
        return 1;
    }

    printf("File Information for: %s\n", filename);
    printf("==========================================\n");

    // a. Inode
    printf("%-25s: %lu\n", "Inode", sb.st_ino);

    // b. Number of hard links
    printf("%-25s: %lu\n", "Hard links", sb.st_nlink);

    // c. UID
    struct passwd *pw = getpwuid(sb.st_uid);
    printf("%-25s: %u (%s)\n", "UID", sb.st_uid, pw ? pw->pw_name : "unknown");

    // d. GID
    struct group *gr = getgrgid(sb.st_gid);
    printf("%-25s: %u (%s)\n", "GID", sb.st_gid, gr ? gr->gr_name : "unknown");

    // e. Size
    printf("%-25s: %ld bytes\n", "Size", sb.st_size);

    // f. Block size
    printf("%-25s: %ld bytes\n", "Block size", sb.st_blksize);

    // g. Number of blocks
    printf("%-25s: %ld\n", "Number of blocks", sb.st_blocks);

    // File type
    printf("%-25s: ", "File type");
    switch (sb.st_mode & S_IFMT) {
        case S_IFREG:  printf("Regular file"); break;
        case S_IFDIR:  printf("Directory"); break;
        case S_IFLNK:  printf("Symbolic link"); break;
        case S_IFIFO:  printf("FIFO/pipe"); break;
        case S_IFSOCK: printf("Socket"); break;
        case S_IFCHR:  printf("Character device"); break;
        case S_IFBLK:  printf("Block device"); break;
        default:       printf("Unknown"); break;
    }
    printf("\n");

    // Permissions
    printf("%-25s: %o\n", "Permissions", sb.st_mode & 0777);

    // h. Time of last access
    print_time("Last access (atime)", sb.st_atime);

    // i. Time of last modification
    print_time("Last modification (mtime)", sb.st_mtime);

    // j. Time of last change
    print_time("Last status change (ctime)", sb.st_ctime);

    return 0;
}

/* Commands to run:
   gcc "9 File Information Extraction.c" -o 9_file_info
   ./9_file_info "9 File Information Extraction.c"
   ./9_file_info /etc/passwd
   ./9_file_info /dev/null
*/
/*
 * Output
 * Command: gcc 9i.c -o 9i
 *
 * Usage: /tmp/ss_run_9i_48157 <filename>
 */

