/*
48. Semaphore Creation and Initialization: Write a program to create a semaphore and initialize its value:
a. Create a binary semaphore
b. Create a counting semaphore
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void create_binary_semaphore() {
    printf("=== a. Binary Semaphore ===\n");
    key_t key = ftok(".", 'B');
    if (key == -1) { perror("ftok"); exit(1); }
    
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); exit(1); }
    
    union semun arg;
    arg.val = 1; // Binary semaphore: 1 = available, 0 = locked
    
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
    } else {
        printf("Binary semaphore created: ID=%d, initial value=1\n", semid);
    }
    
    // Verify
    int val = semctl(semid, 0, GETVAL);
    printf("Current value: %d\n", val);
    
    // Clean up
    semctl(semid, 0, IPC_RMID);
    printf("Binary semaphore removed\n");
}

void create_counting_semaphore() {
    printf("\n=== b. Counting Semaphore ===\n");
    key_t key = ftok(".", 'C');
    if (key == -1) { perror("ftok"); exit(1); }
    
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); exit(1); }
    
    union semun arg;
    arg.val = 5; // Counting semaphore: 5 resources available
    
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
    } else {
        printf("Counting semaphore created: ID=%d, initial value=5\n", semid);
    }
    
    // Verify
    int val = semctl(semid, 0, GETVAL);
    printf("Current value: %d\n", val);
    
    // Clean up
    semctl(semid, 0, IPC_RMID);
    printf("Counting semaphore removed\n");
}

int main() {
    create_binary_semaphore();
    create_counting_semaphore();
    return 0;
}
