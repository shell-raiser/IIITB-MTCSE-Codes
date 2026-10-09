/*
26. Executing ls -Rl Using Various exec System Calls: Write a program to execute ls -Rl using the execl system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    printf("=== Using execl ===\n");
    execl("/bin/ls", "ls", "-Rl", (char *)NULL);
    perror("execl failed");
    return 1;
}

/*
 * Output
 * Command: gcc 26a.c -o 26a && ./26a
 *
 * total 8
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 .
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 ..
 * -rw-r--r--  1 root root 0 Sep 22 00:00 26a.c
 */
