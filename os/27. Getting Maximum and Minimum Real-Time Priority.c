/*
27. Getting Maximum and Minimum Real-Time Priority: Develop a program to retrieve the maximum and minimum real-time priority.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

int main() {
    int max_prio = sched_get_priority_max(SCHED_FIFO);
    int min_prio = sched_get_priority_min(SCHED_FIFO);
    
    if (max_prio == -1 || min_prio == -1) {
        perror("sched_get_priority failed");
        exit(1);
    }
    
    printf("Real-time priority range (SCHED_FIFO):\n");
    printf("  Maximum: %d\n", max_prio);
    printf("  Minimum: %d\n", min_prio);
    
    // Also check SCHED_RR
    max_prio = sched_get_priority_max(SCHED_RR);
    min_prio = sched_get_priority_min(SCHED_RR);
    
    if (max_prio != -1 && min_prio != -1) {
        printf("Real-time priority range (SCHED_RR):\n");
        printf("  Maximum: %d\n", max_prio);
        printf("  Minimum: %d\n", min_prio);
    }
    
    // Check SCHED_OTHER (normal)
    max_prio = sched_get_priority_max(SCHED_OTHER);
    min_prio = sched_get_priority_min(SCHED_OTHER);
    printf("Normal priority range (SCHED_OTHER):\n");
    printf("  Maximum: %d\n", max_prio);
    printf("  Minimum: %d\n", min_prio);
    
    return 0;
}
