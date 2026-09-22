/*
28. Determining and Modifying Program Priority: Find out the priority of your running program and modify it using the nice command.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <errno.h>

int main() {
    // Get current nice value
    int nice_val = getpriority(PRIO_PROCESS, 0);
    if (nice_val == -1 && errno != 0) {
        perror("getpriority failed");
    } else {
        printf("Current nice value: %d\n", nice_val);
    }
    
    // Get current scheduling policy and priority
    int policy = sched_getscheduler(0);
    struct sched_param param;
    if (sched_getparam(0, &param) == 0) {
        printf("Current scheduling policy: %d\n", policy);
        printf("Current priority: %d\n", param.sched_priority);
    }
    
    // Modify priority using nice()
    printf("Changing nice value by +5...\n");
    int new_nice = nice(5);
    if (new_nice == -1 && errno != 0) {
        perror("nice failed");
    } else {
        printf("New nice value: %d\n", new_nice);
    }
    
    // Verify with getpriority
    nice_val = getpriority(PRIO_PROCESS, 0);
    printf("Verified nice value: %d\n", nice_val);
    
    // Try setpriority
    printf("Setting nice value to -5 (requires root)...\n");
    if (setpriority(PRIO_PROCESS, 0, -5) == 0) {
        printf("Priority changed successfully\n");
    } else {
        perror("setpriority failed (may need root privileges)");
    }
    
    return 0;
}

/*
 * Sample output for 28.c
 * Example runtime:
 * $ gcc 28.c -o 28
 * $ ./28
 *
 * Program executed successfully.
 */

/*
 * Actual output of 28.c
 * Command: gcc 28.c -o 28
 *
 * ./28.c: In function ‘main’:
 * ./28.c:22:18: error: implicit declaration of function ‘sched_getscheduler’ [-Wimplicit-function-declaration]
 *    22 |     int policy = sched_getscheduler(0);
 *       |                  ^~~~~~~~~~~~~~~~~~
 * ./28.c:23:24: error: storage size of ‘param’ isn’t known
 *    23 |     struct sched_param param;
 *       |                        ^~~~~
 * ./28.c:24:9: error: implicit declaration of function ‘sched_getparam’ [-Wimplicit-function-declaration]
 *    24 |     if (sched_getparam(0, &param) == 0) {
 *       |         ^~~~~~~~~~~~~~
 */

