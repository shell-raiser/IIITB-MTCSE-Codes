/*
59. Signal Handling: Write separate programs using the signal system call to catch the following signals:
a. SIGSEGV
b. SIGINT
c. SIGFPE
d. SIGALRM (using the alarm system call)
e. SIGALRM (using the setitimer system call)
f. SIGVTALRM (using the setitimer system call)
g. SIGPROF (using the setitimer system call)
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void sigsegv_handler(int sig) {
    printf("Caught SIGSEGV (Segmentation fault)!\n");
    exit(1);
}

void sigint_handler(int sig) {
    printf("Caught SIGINT (Ctrl+C)!\n");
    exit(0);
}

void sigfpe_handler(int sig) {
    printf("Caught SIGFPE (Floating point exception)!\n");
    exit(1);
}

void sigalrm_handler(int sig) {
    printf("Caught SIGALRM!\n");
}

void sigvtalrm_handler(int sig) {
    printf("Caught SIGVTALRM!\n");
}

void sigprof_handler(int sig) {
    printf("Caught SIGPROF!\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <a|b|c|d|e|f|g>\n", argv[0]);
        printf("  a - SIGSEGV\n");
        printf("  b - SIGINT\n");
        printf("  c - SIGFPE\n");
        printf("  d - SIGALRM (alarm)\n");
        printf("  e - SIGALRM (setitimer)\n");
        printf("  f - SIGVTALRM (setitimer)\n");
        printf("  g - SIGPROF (setitimer)\n");
        exit(1);
    }
    
    switch(argv[1][0]) {
        case 'a':
            signal(SIGSEGV, sigsegv_handler);
            printf("SIGSEGV handler installed. Causing segfault...\n");
            int *p = NULL;
            *p = 42; // Cause segfault
            break;
            
        case 'b':
            signal(SIGINT, sigint_handler);
            printf("SIGINT handler installed. Press Ctrl+C...\n");
            while (1) pause();
            break;
            
        case 'c':
            signal(SIGFPE, sigfpe_handler);
            printf("SIGFPE handler installed. Causing divide by zero...\n");
            int x = 1, y = 0;
            int z = x / y; // Cause FPE
            break;
            
        case 'd':
            signal(SIGALRM, sigalrm_handler);
            printf("SIGALRM handler installed. Setting alarm for 3 seconds...\n");
            alarm(3);
            while (1) pause();
            break;
            
        case 'e':
            signal(SIGALRM, sigalrm_handler);
            {
                struct itimerval timer;
                timer.it_value.tv_sec = 3;
                timer.it_value.tv_usec = 0;
                timer.it_interval.tv_sec = 3;
                timer.it_interval.tv_usec = 0;
                setitimer(ITIMER_REAL, &timer, NULL);
                printf("SIGALRM (setitimer) handler installed. Timer set for 3s interval...\n");
                while (1) pause();
            }
            break;
            
        case 'f':
            signal(SIGVTALRM, sigvtalrm_handler);
            {
                struct itimerval timer;
                timer.it_value.tv_sec = 3;
                timer.it_value.tv_usec = 0;
                timer.it_interval.tv_sec = 3;
                timer.it_interval.tv_usec = 0;
                setitimer(ITIMER_VIRTUAL, &timer, NULL);
                printf("SIGVTALRM handler installed. Timer set for 3s virtual time...\n");
                while (1); // Busy loop
            }
            break;
            
        case 'g':
            signal(SIGPROF, sigprof_handler);
            {
                struct itimerval timer;
                timer.it_value.tv_sec = 3;
                timer.it_value.tv_usec = 0;
                timer.it_interval.tv_sec = 3;
                timer.it_interval.tv_usec = 0;
                setitimer(ITIMER_PROF, &timer, NULL);
                printf("SIGPROF handler installed. Timer set for 3s profile time...\n");
                while (1); // Busy loop
            }
            break;
            
        default:
            printf("Invalid option\n");
    }
    
    return 0;
}
