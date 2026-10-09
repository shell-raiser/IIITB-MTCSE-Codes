/*
26. Executing ls -Rl Using Various exec System Calls: Write a program to execute ls -Rl using the execvp system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    printf("=== Using execvp ===\n");
    char *args[] = {"ls", "-Rl", NULL};
    execvp("ls", args);
    perror("execvp failed");
    return 1;
}

/*
 * Output
 * Command: gcc 26e.c -o 26e && ./26e
 *
 * total 8
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 .
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 ..
 * -rw-r--r--  1 root root 0 Sep 22 00:00 26e.c
 */
