/*
26. Executing ls -Rl Using Various exec System Calls: Write a program to execute ls -Rl using the following system calls:
a. execl
b. execlp
c. execle
d. execv
e. execvp
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void run_execl() {
    printf("=== Using execl ===\n");
    execl("/bin/ls", "ls", "-Rl", (char *)NULL);
    perror("execl failed");
}

void run_execlp() {
    printf("=== Using execlp ===\n");
    execlp("ls", "ls", "-Rl", (char *)NULL);
    perror("execlp failed");
}

void run_execle() {
    printf("=== Using execle ===\n");
    char *env[] = { "PATH=/bin:/usr/bin", NULL };
    execle("/bin/ls", "ls", "-Rl", (char *)NULL, env);
    perror("execle failed");
}

void run_execv() {
    printf("=== Using execv ===\n");
    char *args[] = { "ls", "-Rl", NULL };
    execv("/bin/ls", args);
    perror("execv failed");
}

void run_execvp() {
    printf("=== Using execvp ===\n");
    char *args[] = { "ls", "-Rl", NULL };
    execvp("ls", args);
    perror("execvp failed");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <a|b|c|d|e>\n", argv[0]);
        printf("  a - execl\n  b - execlp\n  c - execle\n  d - execv\n  e - execvp\n");
        exit(1);
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        switch(argv[1][0]) {
            case 'a': run_execl(); break;
            case 'b': run_execlp(); break;
            case 'c': run_execle(); break;
            case 'd': run_execv(); break;
            case 'e': run_execvp(); break;
            default: printf("Invalid option\n"); exit(1);
        }
    } else {
        wait(NULL);
    }
    return 0;
}
