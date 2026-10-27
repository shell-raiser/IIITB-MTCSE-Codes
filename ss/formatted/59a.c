/*
59. Signal Handling using signal()
 a. Catch SIGSEGV
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sigsegv_handler(int sig) {
    printf("Caught SIGSEGV (Segmentation fault), signal number = %d\n", sig);
    printf("Note: returning from a SIGSEGV handler re-executes the faulting "
           "instruction and loops forever, so we exit here.\n");
    exit(1);
}

int main(void) {
    signal(SIGSEGV, sigsegv_handler);
    printf("SIGSEGV handler installed. Causing a segfault...\n");
    fflush(stdout);

    volatile int *p = NULL;
    *p = 42;                /* deliberate invalid write */

    printf("UNREACHABLE\n");
    return 0;
}
/*
 * Output
 * Command: gcc 59a.c -o 59a && ./59a
 *
 * SIGSEGV handler installed. Causing a segfault...
 * Caught SIGSEGV (Segmentation fault), signal number = 11
 * Note: returning from a SIGSEGV handler re-executes the faulting
 * instruction and loops forever, so we exit here.
 */
