/*
34. Executing ls -l | wc: Write a program to execute ls -l | wc using:
a. dup
b. dup2
c. fcntl
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void run_with_dup() {
    printf("=== Using dup ===\n");
    int fd[2];
    pipe(fd);
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1: ls -l
        close(fd[0]);
        dup(fd[1]); // Duplicate write end to stdout
        close(fd[1]);
        execlp("ls", "ls", "-l", NULL);
        perror("execlp ls failed");
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2: wc
        close(fd[1]);
        dup(fd[0]); // Duplicate read end to stdin
        close(fd[0]);
        execlp("wc", "wc", NULL);
        perror("execlp wc failed");
        exit(1);
    }
    
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void run_with_dup2() {
    printf("=== Using dup2 ===\n");
    int fd[2];
    pipe(fd);
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execlp("ls", "ls", "-l", NULL);
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        execlp("wc", "wc", NULL);
        exit(1);
    }
    
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void run_with_fcntl() {
    printf("=== Using fcntl ===\n");
    int fd[2];
    pipe(fd);
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(fd[0]);
        fcntl(fd[1], F_DUPFD, STDOUT_FILENO); // Duplicate to stdout
        close(fd[1]);
        execlp("ls", "ls", "-l", NULL);
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd[1]);
        fcntl(fd[0], F_DUPFD, STDIN_FILENO); // Duplicate to stdin
        close(fd[0]);
        execlp("wc", "wc", NULL);
        exit(1);
    }
    
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <a|b|c>\n", argv[0]);
        printf("  a - dup\n  b - dup2\n  c - fcntl\n");
        exit(1);
    }
    
    switch(argv[1][0]) {
        case 'a': run_with_dup(); break;
        case 'b': run_with_dup2(); break;
        case 'c': run_with_fcntl(); break;
        default: printf("Invalid option\n"); exit(1);
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 34.c -o 34
 *
 * Usage: /tmp/ss_run_34_48157 <a|b|c>
 *   a - dup
 *   b - dup2
 *   c - fcntl
 */

