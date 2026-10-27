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
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define ITER 5

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
    /* --- semaphore --- */
    key_t sem_key = ftok(".", 'T');
    if (sem_key == -1) { perror("ftok(sem)"); return 1; }

    int semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1 && errno == EEXIST) {
        /* stale semaphore from an earlier aborted run: drop and recreate */
        semid = semget(sem_key, 1, 0);
        if (semid != -1) semctl(semid, 0, IPC_RMID);
        semid = semget(sem_key, 1, IPC_CREAT | 0666);
    }
    if (semid == -1) { perror("semget"); return 1; }

    union semun arg;
    arg.val = 1;                       /* binary semaphore */
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        return 1;
    }

    /* --- shared ticket counter --- */
    key_t shm_key = ftok(".", 'S');
    if (shm_key == -1) { perror("ftok(shm)"); return 1; }

    int shmid = shmget(shm_key, sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1 && errno == EEXIST) {
        shmid = shmget(shm_key, sizeof(int), 0);
        if (shmid != -1) shmctl(shmid, IPC_RMID, NULL);
        shmid = shmget(shm_key, sizeof(int), IPC_CREAT | 0666);
    }
    if (shmid == -1) { perror("shmget"); return 1; }

    int *ticket = (int *)shmat(shmid, NULL, 0);
    if (ticket == (void *)-1) { perror("shmat"); return 1; }
    *ticket = 100;

    printf("=== a. Ticket Number Creation using a Semaphore ===\n");
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    const char *who = (pid == 0) ? "Child " : "Parent";
    for (int i = 0; i < ITER; i++) {
        if (sem_wait_op(semid, 0) == -1) { perror("semop wait"); break; }

        int t = *ticket;               /* critical section */
        printf("%s: Ticket %d\n", who, t);
        *ticket = t + 1;
        fflush(stdout);

        if (sem_signal_op(semid, 0) == -1) { perror("semop signal"); break; }
        usleep(100000);
    }

    shmdt(ticket);

    if (pid == 0) {
        _exit(0);                      /* child must not remove IPC */
    }

    waitpid(pid, NULL, 0);

    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    return 0;
}
/*
 * Output
 * Command: gcc 49a.c -o 49a && ./49a
 *
 * === a. Ticket Number Creation using a Semaphore ===
 * Parent: Ticket 100
 * Child : Ticket 101
 * Parent: Ticket 102
 * Child : Ticket 103
 * Child : Ticket 104
 * Parent: Ticket 105
 * Child : Ticket 106
 * Parent: Ticket 107
 * Child : Ticket 108
 * Parent: Ticket 109
 *
 * Tickets 100..109 are handed out exactly once each, in order, with no duplicates.
 */
