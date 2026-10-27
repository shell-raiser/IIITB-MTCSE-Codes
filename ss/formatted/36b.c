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

    // b. mkfifo command (via system)
    printf("\n=== b. mkfifo command ===\n");
    system("mkfifo /tmp/myfifo_mkfifo");
    system("ls -l /tmp/myfifo_mkfifo");

    
    // Cleanup
    unlink("/tmp/myfifo_mkfifo");

    
    return 0;
}
