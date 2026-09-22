/*
26. Executing ls -Rl Using Various exec System Calls: Write a program to execute ls -Rl using the execv system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    printf("=== Using execv ===\n");
    char *args[] = {"ls", "-Rl", NULL};
    execv("/bin/ls", args);
    perror("execv failed");
    return 1;
}

/*
 * Output
 * Command: gcc 26d.c -o 26d && ./26d
 *
 * total 8
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 .
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 ..
 * -rw-r--r--  1 root root 0 Sep 22 00:00 26d.c
 */
