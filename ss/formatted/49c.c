/*
49. Semaphore Implementation: Write a program to implement a semaphore to protect any critical section:
a. Rewrite the ticket number creation program using a semaphore
b. Protect shared memory from concurrent write access
c. Protect multiple pseudo resources (maybe two) using a counting semaphore
d. Remove the created semaphore
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define NPROC     4
#define NRESOURCE 2

static int sem_wait_op(int semid, int semnum) {
    struct sembuf op = {(unsigned short)semnum, -1, 0};
    while (semop(semid, &op, 1) == -1) {
        if (errno != EINTR) return -1;
    }
    return 0;
}

static int sem_signal_op(int semid, int semnum) {
    struct sembuf op = {(unsigned short)semnum, 1, 0};
    while (semop(semid, &op, 1) == -1) {
        if (errno != EINTR) return -1;
    }
    return 0;
}

int main(void) {
    key_t key = ftok(".", 'R');
    if (key == -1) { perror("ftok"); return 1; }

    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1 && errno == EEXIST) {
        semid = semget(key, 1, 0);
        if (semid != -1) semctl(semid, 0, IPC_RMID);
        semid = semget(key, 1, IPC_CREAT | 0666);
    }
    if (semid == -1) { perror("semget"); return 1; }

    union semun arg;
    arg.val = NRESOURCE;               /* counting semaphore: 2 units */
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("semctl SETVAL"); return 1; }

    printf("=== c. Counting Semaphore protecting %d pseudo resources ===\n", NRESOURCE);
    fflush(stdout);

    pid_t pids[NPROC];
    for (int i = 0; i < NPROC; i++) {
        pids[i] = fork();
        if (pids[i] == -1) { perror("fork"); return 1; }

        if (pids[i] == 0) {
            printf("Process %d: waiting for a resource\n", i);
            fflush(stdout);

            if (sem_wait_op(semid, 0) == -1) { perror("semop wait"); _exit(1); }

            printf("Process %d: acquired resource\n", i);
            fflush(stdout);
            sleep(1);
            printf("Process %d: releasing resource\n", i);
            fflush(stdout);

            if (sem_signal_op(semid, 0) == -1) { perror("semop signal"); _exit(1); }
            _exit(0);
        }
    }

    for (int i = 0; i < NPROC; i++) waitpid(pids[i], NULL, 0);

    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");
    printf("Semaphore removed\n");
    return 0;
}
/*
 * Output
 * Command: gcc 49c.c -o 49c && ./49c
 *
 * === c. Counting Semaphore protecting 2 pseudo resources ===
 * Process 0: waiting for a resource
 * Process 1: waiting for a resource
 * Process 2: waiting for a resource
 * Process 3: waiting for a resource
 * Process 0: acquired resource
 * Process 1: acquired resource
 * Process 0: releasing resource
 * Process 1: releasing resource
 * Process 2: acquired resource
 * Process 3: acquired resource
 * Process 2: releasing resource
 * Process 3: releasing resource
 * Semaphore removed
 *
 * At most 2 processes hold the resource at any time.
 */
