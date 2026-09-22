/*
61. Signal Handling with sigaction: Write separate programs using the sigaction system call to catch the following signals:
a. SIGSEGV
b. SIGINT
c. SIGFPE
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigsegv_handler(int sig, siginfo_t *info, void *context) {
    printf("Caught SIGSEGV (Segmentation fault)!\n");
    printf("  Faulting address: %p\n", info->si_addr);
    exit(1);
}

void sigint_handler(int sig, siginfo_t *info, void *context) {
    printf("Caught SIGINT (Ctrl+C)!\n");
    exit(0);
}

void sigfpe_handler(int sig, siginfo_t *info, void *context) {
    printf("Caught SIGFPE (Floating point exception)!\n");
    printf("  Code: %d\n", info->si_code);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <a|b|c>\n", argv[0]);
        printf("  a - SIGSEGV\n  b - SIGINT\n  c - SIGFPE\n");
        exit(1);
    }
    
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    
    switch(argv[1][0]) {
        case 'a':
            sa.sa_sigaction = sigsegv_handler;
            sigaction(SIGSEGV, &sa, NULL);
            printf("SIGSEGV handler installed with sigaction. Causing segfault...\n");
            int *p = NULL;
            *p = 42;
            break;
            
        case 'b':
            sa.sa_sigaction = sigint_handler;
            sigaction(SIGINT, &sa, NULL);
            printf("SIGINT handler installed with sigaction. Press Ctrl+C...\n");
            while (1) pause();
            break;
            
        case 'c':
            sa.sa_sigaction = sigfpe_handler;
            sigaction(SIGFPE, &sa, NULL);
            printf("SIGFPE handler installed with sigaction. Causing divide by zero...\n");
            int x = 1, y = 0;
            int z = x / y;
            break;
            
        default:
            printf("Invalid option\n");
    }
    
    return 0;
}
