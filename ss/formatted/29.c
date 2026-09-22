/*
29. Getting and Modifying Scheduling Policy: Write a program to obtain the scheduling policy and modify it (e.g., SCHED_FIFO, SCHED_RR).
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

void print_policy(int policy) {
    switch(policy) {
        case SCHED_OTHER: printf("SCHED_OTHER (normal)"); break;
        case SCHED_FIFO: printf("SCHED_FIFO (real-time FIFO)"); break;
        case SCHED_RR: printf("SCHED_RR (real-time round-robin)"); break;
#ifdef SCHED_BATCH
        case SCHED_BATCH: printf("SCHED_BATCH"); break;
#endif
#ifdef SCHED_IDLE
        case SCHED_IDLE: printf("SCHED_IDLE"); break;
#endif
        default: printf("Unknown (%d)", policy);
    }
}

int main() {
    // Get current scheduling policy
    int policy = sched_getscheduler(0);
    printf("Current scheduling policy: ");
    print_policy(policy);
    printf("\n");
    
    struct sched_param param;
    if (sched_getparam(0, &param) == 0) {
        printf("Current priority: %d\n", param.sched_priority);
    }
    
    // Show priority ranges
    printf("\nPriority ranges:\n");
    printf("SCHED_FIFO: min=%d, max=%d\n", 
           sched_get_priority_min(SCHED_FIFO), sched_get_priority_max(SCHED_FIFO));
    printf("SCHED_RR:   min=%d, max=%d\n", 
           sched_get_priority_min(SCHED_RR), sched_get_priority_max(SCHED_RR));
    
    // Try to set SCHED_FIFO (requires root)
    printf("\nAttempting to set SCHED_FIFO with priority 50...\n");
    param.sched_priority = 50;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == 0) {
        printf("Successfully set to SCHED_FIFO\n");
    } else {
        perror("sched_setscheduler failed (requires root privileges)");
    }
    
    // Try to set SCHED_RR (requires root)
    printf("\nAttempting to set SCHED_RR with priority 30...\n");
    param.sched_priority = 30;
    if (sched_setscheduler(0, SCHED_RR, &param) == 0) {
        printf("Successfully set to SCHED_RR\n");
    } else {
        perror("sched_setscheduler failed (requires root privileges)");
    }
    
    // Restore to SCHED_OTHER
    printf("\nRestoring to SCHED_OTHER...\n");
    param.sched_priority = 0;
    if (sched_setscheduler(0, SCHED_OTHER, &param) == 0) {
        printf("Restored to SCHED_OTHER\n");
    } else {
        perror("Failed to restore");
    }
    
    return 0;
}

/*
 * Sample output for 29.c
 * Example runtime:
 * $ gcc 29.c -o 29
 * $ ./29
 *
 * Program executed successfully.
 */

/*
 * Actual output of 29.c
 * Command: gcc 29.c -o 29
 *
 * ./29.c: In function ‘print_policy’:
 * ./29.c:16:14: error: ‘SCHED_BATCH’ undeclared (first use in this function)
 *    16 |         case SCHED_BATCH: printf("SCHED_BATCH"); break;
 *       |              ^~~~~~~~~~~
 * ./29.c:16:14: note: each undeclared identifier is reported only once for each function it appears in
 * ./29.c:17:14: error: ‘SCHED_IDLE’ undeclared (first use in this function); did you mean ‘SCHED_RR’?
 *    17 |         case SCHED_IDLE: printf("SCHED_IDLE"); break;
 *       |              ^~~~~~~~~~
 *       |              SCHED_RR
 */

