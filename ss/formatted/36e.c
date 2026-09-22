/*
36. FIFO File Creation: Create a FIFO file using:
a. mknod command
b. mkfifo command
c. Use strace command to determine which command (mknod or mkfifo) is more efficient.
d. mknod system call
e. mkfifo library function
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    // a. mknod command (via system)
    printf("=== a. mknod command ===\n");
    system("mknod /tmp/myfifo_mknod p");
    system("ls -l /tmp/myfifo_mknod");
    
    // b. mkfifo command (via system)
    printf("\n=== b. mkfifo command ===\n");
    system("mkfifo /tmp/myfifo_mkfifo");
    system("ls -l /tmp/myfifo_mkfifo");
    
    // c. strace comparison (run separately)
    printf("\n=== c. strace comparison ===\n");
    printf("Run manually:\n");
    printf("  strace -c mknod /tmp/test_mknod p\n");
    printf("  strace -c mkfifo /tmp/test_mkfifo\n");
    
    // d. mknod system call
    printf("\n=== d. mknod system call ===\n");
    if (mknod("/tmp/myfifo_syscall", S_IFIFO | 0666, 0) == 0) {
        printf("Created FIFO with mknod syscall\n");
        system("ls -l /tmp/myfifo_syscall");
    } else {
        perror("mknod syscall failed");
    }
    
    // e. mkfifo library function
    printf("\n=== e. mkfifo library function ===\n");
    if (mkfifo("/tmp/myfifo_libfunc", 0666) == 0) {
        printf("Created FIFO with mkfifo library function\n");
        system("ls -l /tmp/myfifo_libfunc");
    } else {
        perror("mkfifo library function failed");
    }
    
    // Cleanup
    unlink("/tmp/myfifo_mknod");
    unlink("/tmp/myfifo_mkfifo");
    unlink("/tmp/myfifo_syscall");
    unlink("/tmp/myfifo_libfunc");
    
    return 0;
}

/*
 * Sample output for 36e.c
 * Example runtime:
 * $ gcc 36e.c -o 36e
 * $ ./36e
 *
 * Created FIFO with mkfifo library function
 */

/*
 * Actual output of 36e.c
 * Command: gcc 36e.c -o 36e
 *
 * prw-r--r-- 1 kss kss 0 Sep 22 21:08 /tmp/myfifo_mknod
 * prw-r--r-- 1 kss kss 0 Sep 22 21:08 /tmp/myfifo_mkfifo
 * prw-r--r-- 1 kss kss 0 Sep 22 21:08 /tmp/myfifo_syscall
 * prw-r--r-- 1 kss kss 0 Sep 22 21:08 /tmp/myfifo_libfunc
 * === a. mknod command ===
 *
 * === b. mkfifo command ===
 *
 * === c. strace comparison ===
 * Run manually:
 *   strace -c mknod /tmp/test_mknod p
 *   strace -c mkfifo /tmp/test_mkfifo
 *
 * === d. mknod system call ===
 * Created FIFO with mknod syscall
 *
 * === e. mkfifo library function ===
 * Created FIFO with mkfifo library function
 */

