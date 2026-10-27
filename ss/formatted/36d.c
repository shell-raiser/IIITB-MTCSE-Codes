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
    // d. mknod system call
    printf("\n=== d. mknod system call ===\n");
    if (mknod("/tmp/myfifo_syscall", S_IFIFO | 0666, 0) == 0) {
        printf("Created FIFO with mknod syscall\n");
        system("ls -l /tmp/myfifo_syscall");
    } else {
        perror("mknod syscall failed");
    }
    

    unlink("/tmp/myfifo_syscall");
    
    return 0;
}
