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
    
    // Cleanup
    unlink("/tmp/myfifo_mknod");
 
    return 0;
}
/*
 * Output
 * Command: gcc 36a.c -o 36a
 *
 */

