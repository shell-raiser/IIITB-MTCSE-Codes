/*
26. Executing ls -Rl Using Various exec System Calls: Write a program to execute ls -Rl using the execlp system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    printf("=== Using execlp ===\n");
    execlp("ls", "ls", "-Rl", (char *)NULL);
    perror("execlp failed");
    return 1;
}

/*
 * Output
 * Command: gcc 26b.c -o 26b && ./26b
 *
 * total 8
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 .
 * drwxr-xr-x  4 root root 4096 Sep 22 00:00 ..
 * -rw-r--r--  1 root root 0 Sep 22 00:00 26b.c
 */
