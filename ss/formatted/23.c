/*
23. Creating an Orphan Process: Develop a program to create an orphan process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process - sleep longer than parent
        printf("Child (PID: %d) started, parent is %d\n", getpid(), getppid());
        sleep(5);
        printf("Child (PID: %d) now orphan, new parent is %d\n", getpid(), getppid());
        printf("Child exiting\n");
    } else {
        // Parent process - exit immediately
        printf("Parent (PID: %d) exiting immediately\n", getpid());
        // Parent exits, child becomes orphan (adopted by init/systemd)
    }
    return 0;
}
/*
 * Output
 * Command: gcc 23.c -o 23
 *
 * Parent (PID: 48355) exiting immediately
 * Child (PID: 48356) started, parent is 48355
 * Child (PID: 48356) now orphan, new parent is 14481
 * Child exiting
 */

