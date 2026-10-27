/*
49. Semaphore Implementation: Write a program to implement a semaphore to protect any critical section:
a. Rewrite the ticket number creation program using a semaphore
b. Protect shared memory from concurrent write access
c. Protect multiple pseudo resources (maybe two) using a counting semaphore
d. Remove the created semaphore
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define SHM_SIZE 256
#define ITER     3

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
    key_t sem_key = ftok(".", 'M');
    if (sem_key == -1) { perror("ftok(sem)"); return 1; }

    int semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1 && errno == EEXIST) {
        semid = semget(sem_key, 1, 0);
        if (semid != -1) semctl(semid, 0, IPC_RMID);
        semid = semget(sem_key, 1, IPC_CREAT | 0666);
    }
    if (semid == -1) { perror("semget"); return 1; }

    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("semctl SETVAL"); return 1; }

    key_t shm_key = ftok(".", 'D');
    if (shm_key == -1) { perror("ftok(shm)"); return 1; }

    int shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1 && errno == EEXIST) {
        shmid = shmget(shm_key, SHM_SIZE, 0);
        if (shmid != -1) shmctl(shmid, IPC_RMID, NULL);
        shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    }
    if (shmid == -1) { perror("shmget"); return 1; }

    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) { perror("shmat"); return 1; }
    shm[0] = '\0';

    printf("=== b. Protecting Shared Memory from Concurrent Writes ===\n");
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    const char *who = (pid == 0) ? "Child " : "Parent";
    for (int i = 0; i < ITER; i++) {
        if (sem_wait_op(semid, 0) == -1) { perror("semop wait"); break; }

        /* critical section: full read-modify-write of the shared segment */
        snprintf(shm, SHM_SIZE, "%s write %d", who, i);
        printf("%s wrote: %s\n", who, shm);
        fflush(stdout);

        if (sem_signal_op(semid, 0) == -1) { perror("semop signal"); break; }
        usleep(100000);
    }

    shmdt(shm);

    if (pid == 0) _exit(0);

    waitpid(pid, NULL, 0);

    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    return 0;
}
/*
 * Output
 * Command: gcc 49b.c -o 49b && ./49b
 *
 * === b. Protecting Shared Memory from Concurrent Writes ===
 * Parent wrote: Parent write 0
 * Child  wrote: Child  write 0
 * Parent wrote: Parent write 1
 * Child  wrote: Child  write 1
 * Parent wrote: Parent write 2
 * Child  wrote: Child  write 2
 *
 * Each write is complete and never interleaved with the other process.
 */
