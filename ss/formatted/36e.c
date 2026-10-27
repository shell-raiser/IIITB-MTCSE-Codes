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
 
    // e. mkfifo library function
    printf("\n=== e. mkfifo library function ===\n");
    if (mkfifo("/tmp/myfifo_libfunc", 0666) == 0) {
        printf("Created FIFO with mkfifo library function\n");
        system("ls -l /tmp/myfifo_libfunc");
    } else {
        perror("mkfifo library function failed");
    }
    
    unlink("/tmp/myfifo_libfunc");
    
    return 0;
}


