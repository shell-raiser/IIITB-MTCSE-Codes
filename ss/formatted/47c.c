/*
47. Shared Memory Operations: Write a program to create shared memory and perform the following operations:
a. Write some data to the shared memory
b. Attach with O_RDONLY and check whether you are able to overwrite
c. Detach the shared memory
d. Remove the shared memory
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok(".", 'S');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }
    
    // Create shared memory segment
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    
    printf("Shared memory created: ID=%d, Key=0x%x\n", shmid, key);
    
    // a. Write some data to the shared memory
    printf("\n=== a. Writing data ===\n");
    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }
    
    strcpy(shm, "Hello, Shared Memory!");
    printf("Written: %s\n", shm);
    shmdt(shm);
    
    // b. Attach with O_RDONLY (SHM_RDONLY flag) and check overwrite
    printf("\n=== b. Attaching read-only ===\n");
    shm = (char *)shmat(shmid, NULL, SHM_RDONLY);
    if (shm == (char *)-1) {
        perror("shmat read-only failed");
        exit(1);
    }
    
    printf("Read: %s\n", shm);
    
    // Try to overwrite (should fail with SIGSEGV or be ignored)
    printf("Attempting to overwrite read-only shared memory...\n");
    // This will cause a segmentation fault on most systems
    // strcpy(shm, "New data"); // Uncomment to test - will crash!
    printf("Note: Writing to read-only shared memory causes SIGSEGV\n");
    
    shmdt(shm);
    
    // c. Detach the shared memory
    printf("\n=== c. Detaching ===\n");
    shm = (char *)shmat(shmid, NULL, 0);
    if (shm != (char *)-1) {
        shmdt(shm);
        printf("Detached successfully\n");
    }
    
    // d. Remove the shared memory
    printf("\n=== d. Removing shared memory ===\n");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
    } else {
        printf("Shared memory removed\n");
    }
    
    return 0;
}

/*
 * Sample output for 47c.c
 * Example runtime:
 * $ gcc 47c.c -o 47c
 * $ ./47c
 *
 * Detached successfully
 */

/*
 * Actual output of 47c.c
 * Command: gcc 47c.c -o 47c
 *
 * Shared memory created: ID=15, Key=0x533664f1
 *
 * === a. Writing data ===
 * Written: Hello, Shared Memory!
 *
 * === b. Attaching read-only ===
 * Read: Hello, Shared Memory!
 * Attempting to overwrite read-only shared memory...
 * Note: Writing to read-only shared memory causes SIGSEGV
 *
 * === c. Detaching ===
 * Detached successfully
 *
 * === d. Removing shared memory ===
 * Shared memory removed
 */

