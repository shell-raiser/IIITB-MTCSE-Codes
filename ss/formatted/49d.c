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

int semid;
int shmid;
int *ticket_ptr;

// Semaphore operations
void sem_wait(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}

// a. Ticket number creation using semaphore
void ticket_program() {
    printf("=== a. Ticket Number with Semaphore ===\n");
    
    key_t key = ftok(".", 'T');
    semid = semget(key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    
    shmid = shmget(ftok(".", 'S'), sizeof(int), IPC_CREAT | 0666);
    ticket_ptr = (int *)shmat(shmid, NULL, 0);
    *ticket_ptr = 100;
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child
        for (int i = 0; i < 5; i++) {
            sem_wait(semid, 0);
            int t = *ticket_ptr;
            printf("Child: Ticket %d\n", t);
            (*ticket_ptr)++;
            sem_signal(semid, 0);
            sleep(1);
        }
        shmdt(ticket_ptr);
        exit(0);
    }
    
    // Parent
    for (int i = 0; i < 5; i++) {
        sem_wait(semid, 0);
        int t = *ticket_ptr;
        printf("Parent: Ticket %d\n", t);
        (*ticket_ptr)++;
        sem_signal(semid, 0);
        sleep(1);
    }
    
    wait(NULL);
    shmdt(ticket_ptr);
    semctl(semid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);
}

// b. Protect shared memory from concurrent write access
void protect_shared_memory() {
    printf("\n=== b. Protect Shared Memory ===\n");
    
    key_t key = ftok(".", 'M');
    semid = semget(key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    
    shmid = shmget(ftok(".", 'D'), 256, IPC_CREAT | 0666);
    char *shm = (char *)shmat(shmid, NULL, 0);
    
    pid_t pid = fork();
    if (pid == 0) {
        for (int i = 0; i < 3; i++) {
            sem_wait(semid, 0);
            sprintf(shm, "Child write %d", i);
            printf("Child wrote: %s\n", shm);
            sem_signal(semid, 0);
            sleep(1);
        }
        shmdt(shm);
        exit(0);
    }
    
    for (int i = 0; i < 3; i++) {
        sem_wait(semid, 0);
        sprintf(shm, "Parent write %d", i);
        printf("Parent wrote: %s\n", shm);
        sem_signal(semid, 0);
        sleep(1);
    }
    
    wait(NULL);
    shmdt(shm);
    semctl(semid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);
}

// c. Protect multiple pseudo resources using counting semaphore
void counting_semaphore_demo() {
    printf("\n=== c. Counting Semaphore (2 resources) ===\n");
    
    key_t key = ftok(".", 'R');
    semid = semget(key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 2; // 2 resources
    semctl(semid, 0, SETVAL, arg);
    
    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Process %d: Waiting for resource...\n", i);
            sem_wait(semid, 0);
            printf("Process %d: Got resource!\n", i);
            sleep(2);
            printf("Process %d: Releasing resource\n", i);
            sem_signal(semid, 0);
            exit(0);
        }
    }
    
    for (int i = 0; i < 4; i++) wait(NULL);
    
    // d. Remove the created semaphore
    printf("\n=== d. Removing semaphore ===\n");
    semctl(semid, 0, IPC_RMID);
    printf("Semaphore removed\n");
}

int main() {
    ticket_program();
    protect_shared_memory();
    counting_semaphore_demo();
    return 0;
}

/*
 * Sample output for 49d.c
 * Example runtime:
 * $ gcc 49d.c -o 49d
 * $ ./49d
 *
 * Semaphore removed
 */

/*
 * Actual output of 49d.c
 * Command: gcc 49d.c -o 49d
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

