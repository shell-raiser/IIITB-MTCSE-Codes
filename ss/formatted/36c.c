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

    // c. strace comparison (run separately)
    printf("\n=== c. strace comparison ===\n");
    printf("Run manually:\n");
    printf("  strace -c mknod /tmp/test_mknod p\n");
    printf("  strace -c mkfifo /tmp/test_mkfifo\n");

    
    return 0;
}


