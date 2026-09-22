/*
25. Executing an Executable Program:
a. Execute a program using the exec system call.
b. Pass input to an executable program (e.g., execute an executable as $./a.out name).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program_to_execute> [args...]\n", argv[0]);
        printf("Example: %s ./a.out name\n", argv[0]);
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child - execute the program
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    } else {
        // Parent - wait for child
        wait(NULL);
        printf("Child process completed\n");
    }
    return 0;
}
/*
 * Output
 * Command: gcc 25.c -o 25
 *
 * Usage: /tmp/ss_run_25_48157 <program_to_execute> [args...]
 * Example: /tmp/ss_run_25_48157 ./a.out name
 */

