// 1. Creating Different File Types: Utilize both shell commands and system calls to
// create the following types of files:
// a. soft link (using the symlink system call) `ln -s`
// b. hard link (using the link system call) `ln`
// c. FIFO (using either the mkfifo Library Function or the mknod system call) `mknod myfifo p`

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    // Create a target file first
    int fd = open("target.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open target.txt");
        return 1;
    }
    write(fd, "Hello, this is target file\n", 28);
    close(fd);

    // a. Create soft link using symlink system call
    // Shell command: ln -s target.txt softlink.txt
    if (symlink("target.txt", "softlink.txt") == -1) {
        perror("symlink");
    } else {
        printf("Soft link created: softlink.txt -> target.txt\n");
    }

    // b. Create hard link using link system call
    // Shell command: ln target.txt hardlink.txt
    if (link("target.txt", "hardlink.txt") == -1) {
        perror("link");
    } else {
        printf("Hard link created: hardlink.txt -> target.txt\n");
    }

    // c. Create FIFO using mkfifo library function
    // Shell command: mkfifo myfifo
    if (mkfifo("myfifo_mkfifo", 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
        }
    } else {
        printf("FIFO created using mkfifo: myfifo_mkfifo\n");
    }

    // c. Create FIFO using mknod system call
    // Shell command: mknod myfifo_mknod p
    if (mknod("myfifo_mknod", S_IFIFO | 0666, 0) == -1) {
        if (errno != EEXIST) {
            perror("mknod");
        }
    } else {
        printf("FIFO created using mknod: myfifo_mknod\n");
    }

    // Verify file types using stat
    struct stat sb;
    const char *files[] = {"target.txt", "softlink.txt", "hardlink.txt", "myfifo_mkfifo", "myfifo_mknod"};
    const char *desc[] = {"Regular file", "Soft link", "Hard link", "FIFO (mkfifo)", "FIFO (mknod)"};

    for (int i = 0; i < 5; i++) {
        if (stat(files[i], &sb) == 0) {
            printf("\n%s (%s):\n", files[i], desc[i]);
            printf("  Inode: %lu\n", sb.st_ino);
            printf("  Mode: %o\n", sb.st_mode & 0777);
            printf("  File type: ");
            switch (sb.st_mode & S_IFMT) {
                case S_IFREG:  printf("Regular file"); break;
                case S_IFLNK:  printf("Symbolic link"); break;
                case S_IFIFO:  printf("FIFO/pipe"); break;
                default:       printf("Other"); break;
            }
            printf("\n  Hard links: %lu\n", sb.st_nlink);
        }
    }

    return 0;
}

/* Commands to run:
   gcc "1 Creating Different File Types.c" -o 1_file_types
   ./1_file_types
   ls -li target.txt softlink.txt hardlink.txt myfifo_mkfifo myfifo_mknod
   file target.txt softlink.txt hardlink.txt myfifo_mkfifo myfifo_mknod
*/
/*
 * Output
 * Command: gcc 1b.c -o 1b
 *
 * target.txt (Regular file):
 *   Inode: 2057754
 *   Mode: 644
 *   File type: Regular file
 *   Hard links: 2
 *
 * softlink.txt (Soft link):
 *   Inode: 2057754
 *   Mode: 644
 *   File type: Regular file
 *   Hard links: 2
 *
 * hardlink.txt (Hard link):
 *   Inode: 2057754
 *   Mode: 644
 *   File type: Regular file
 *   Hard links: 2
 *
 * myfifo_mkfifo (FIFO (mkfifo)):
 *   Inode: 2057756
 *   Mode: 644
 *   File type: FIFO/pipe
 *   Hard links: 1
 *
 * myfifo_mknod (FIFO (mknod)):
 *   Inode: 2057757
 *   Mode: 644
 *   File type: FIFO/pipe
 *   Hard links: 1
 *
 * symlink: File exists
 * link: File exists
 */

