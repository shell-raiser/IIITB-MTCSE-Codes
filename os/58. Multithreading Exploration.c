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
