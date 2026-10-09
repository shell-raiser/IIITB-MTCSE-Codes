/*
50. Deadlock: Develop a program that intentionally induces a deadlock scenario using semaphores.
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

void sem_wait(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}

int main() {
    // Create two semaphores
    key_t key1 = ftok(".", 'D');
    key_t key2 = ftok(".", 'E');
    
    int semid1 = semget(key1, 1, IPC_CREAT | 0666);
    int semid2 = semget(key2, 1, IPC_CREAT | 0666);
    
    union semun arg;
    arg.val = 1;
    semctl(semid1, 0, SETVAL, arg);
    semctl(semid2, 0, SETVAL, arg);
    
    printf("Created two semaphores: A(%d) and B(%d)\n", semid1, semid2);
    printf("Process 1 will lock A then B\n");
    printf("Process 2 will lock B then A\n");
    printf("This creates a deadlock!\n\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Process 1: Lock A then B
        printf("Process 1: Trying to lock A...\n");
        sem_wait(semid1, 0);
        printf("Process 1: Locked A, sleeping 1s...\n");
        sleep(1);
        printf("Process 1: Trying to lock B...\n");
        sem_wait(semid2, 0);
        printf("Process 1: Locked B (should not reach here)\n");
        sem_signal(semid2, 0);
        sem_signal(semid1, 0);
        exit(0);
    } else {
        // Process 2: Lock B then A
        sleep(1); // Let process 1 get A first
        printf("Process 2: Trying to lock B...\n");
        sem_wait(semid2, 0);
        printf("Process 2: Locked B, sleeping 1s...\n");
        sleep(1);
        printf("Process 2: Trying to lock A...\n");
        sem_wait(semid1, 0);
        printf("Process 2: Locked A (should not reach here)\n");
        sem_signal(semid1, 0);
        sem_signal(semid2, 0);
        
        wait(NULL); // Will hang here due to deadlock
    }
    
    // Cleanup (won't reach due to deadlock)
    semctl(semid1, 0, IPC_RMID);
    semctl(semid2, 0, IPC_RMID);
    
    return 0;
}
/*
 * Output
 * Command: gcc 50.c -o 50
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

