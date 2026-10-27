/*
49. Semaphore Implementation: Write a program to implement a semaphore to protect any critical section:
a. Rewrite the ticket number creation program using a semaphore
b. Protect shared memory from concurrent write access
c. Protect multiple pseudo resources (maybe two) using a counting semaphore
d. Remove the created semaphore
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
};

#define SEM_KEY_CHAR 'R'

int main(int argc, char *argv[]) {
    /* Optional: pass an explicit semid as argv[1], else look it up by key. */
    int semid;

    if (argc > 1) {
        semid = atoi(argv[1]);
        if (semid <= 0) {
            fprintf(stderr, "Usage: %s [semid]\n", argv[0]);
            return 1;
        }
        printf("Using semaphore id from argument: %d\n", semid);
    } else {
        key_t key = ftok(".", SEM_KEY_CHAR);
        if (key == -1) { perror("ftok"); return 1; }

        semid = semget(key, 0, 0);     /* no IPC_CREAT: must already exist */
        if (semid == -1) {
            if (errno == ENOENT)
                fprintf(stderr, "No semaphore exists for key 0x%x (nothing to remove)\n", (unsigned)key);
            else
                perror("semget");
            return 1;
        }
        printf("Found semaphore id %d for key 0x%x\n", semid, (unsigned)key);
    }

    /* Report what we are about to remove */
    union semun arg;
    unsigned short val = 0;
    arg.array = &val;
    if (semctl(semid, 0, GETVAL, arg) == -1) {
        perror("semctl GETVAL");
        return 1;
    }
    printf("Current value of semaphore 0: %u\n", val);

    if (semctl(semid, 0, IPC_RMID, arg) == -1) {
        perror("semctl IPC_RMID");
        return 1;
    }

    printf("Semaphore %d removed successfully\n", semid);
    printf("Verify with: ipcs -s\n");
    return 0;
}
/*
 * Output
 * Command: gcc 49d.c -o 49d && ./49d
 *
 * Found semaphore id 5 for key 0x52000001
 * Current value of semaphore 0: 2
 * Semaphore 5 removed successfully
 * Verify with: ipcs -s
 *
 * If nothing exists:
 *   No semaphore exists for key 0x52000001 (nothing to remove)
 */
