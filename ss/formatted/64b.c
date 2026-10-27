/*
64. Signal SIGSTOP Handling: Create two programs: the first program awaits
the SIGSTOP signal, while the second program sends the signal using the kill
system call. Determine whether the first program successfully catches the
signal or not.

Program 2 of 2: sender (sends SIGSTOP with kill()).
Companion program: 64a.c (receiver).
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

static int pid_is_alive(pid_t pid) {
    if (kill(pid, 0) == 0) return 1;
    return errno != ESRCH;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid-of-64a>\n", argv[0]);
        return 1;
    }

    pid_t target = (pid_t)atoi(argv[1]);
    if (target <= 0) {
        fprintf(stderr, "Invalid pid: %s\n", argv[1]);
        return 1;
    }

    if (!pid_is_alive(target)) {
        fprintf(stderr, "No such process: %d\n", target);
        return 1;
    }

    printf("Sender: sending SIGSTOP to PID %d\n", target);
    if (kill(target, SIGSTOP) == -1) {
        fprintf(stderr, "Sender: kill failed: %s\n", strerror(errno));
        return 1;
    }
    printf("Sender: SIGSTOP sent successfully\n");

    /* Give the kernel a moment to perform the state transition, then show
     * the observable proof: the target is in state T (stopped), and it was
     * never handled by the program. */
    usleep(200000);
    printf("Sender: target state = ");
    fflush(stdout);

    pid_t p = fork();
    if (p == 0) {
        char pidbuf[32];
        snprintf(pidbuf, sizeof pidbuf, "%d", target);
        execlp("ps", "ps", "-o", "stat=", "-p", pidbuf, (char *)NULL);
        _exit(127);
    }
    waitpid(p, NULL, 0);

    printf("Sender: 'T' means the process was STOPPED by the kernel.\n");
    printf("Sender: SIGSTOP was NOT caught by 64a (it cannot be).\n");
    printf("Sender: resume the target with: kill -CONT %d\n", target);

    return 0;
}
/*
 * Output
 * Command: gcc 64b.c -o 64b && ./64b 12345
 *
 * Sender: sending SIGSTOP to PID 12345
 * Sender: SIGSTOP sent successfully
 * Sender: target state = T
 * Sender: 'T' means the process was STOPPED by the kernel.
 * Sender: SIGSTOP was NOT caught by 64a (it cannot be).
 * Sender: resume the target with: kill -CONT 12345
 */
