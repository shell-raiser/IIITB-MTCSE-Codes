/*
58. Multithreading Exploration: Write a simple program to create three threads and print the IDs of the created threads.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg) {
    int id = *(int *)arg;
    pthread_t tid = pthread_self();
    printf("Thread %d: TID = %lu (0x%lx)\n", id, (unsigned long)tid, (unsigned long)tid);
    sleep(1);
    return NULL;
}

int main() {
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};
    
    printf("Main thread TID: %lu\n", (unsigned long)pthread_self());
    printf("Creating 3 threads...\n\n");
    
    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", ret);
            exit(1);
        }
        printf("Created thread %d with pthread_t: %lu\n", i+1, (unsigned long)threads[i]);
    }
    
    // Wait for all threads
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\nAll threads completed\n");
    return 0;
}

/*
 * Sample output for 58.c
 * Example runtime:
 * $ gcc 58.c -o 58
 * $ ./58
 *
 * Program executed successfully.
 */

/*
 * Actual output of 58.c
 * Command: gcc 58.c -o 58
 *
 * Main thread TID: 139701260203840
 * Creating 3 threads...
 *
 * Created thread 1 with pthread_t: 139701256189632
 * Created thread 2 with pthread_t: 139701247796928
 * Thread 1: TID = 139701256189632 (0x7f0ebbbff6c0)
 * Created thread 3 with pthread_t: 139701239404224
 * Thread 3: TID = 139701239404224 (0x7f0ebabfd6c0)
 * Thread 2: TID = 139701247796928 (0x7f0ebb3fe6c0)
 *
 * All threads completed
 */

