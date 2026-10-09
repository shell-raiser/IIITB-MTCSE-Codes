/*
30. Running a Script at a Specific Time with a Daemon Process: Create a program to execute a task at a specific time using a Daemon process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <string.h>

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0); // Parent exits
    
    // Child continues
    if (setsid() < 0) exit(1);
    
    // Fork again to prevent acquiring controlling terminal
    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);
    
    // Change working directory
    chdir("/");
    
    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Redirect to /dev/null
    open("/dev/null", O_RDONLY); // stdin
    open("/dev/null", O_WRONLY); // stdout
    open("/dev/null", O_WRONLY); // stderr
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <HH:MM> <command> [args...]\n", argv[0]);
        printf("Example: %s 14:30 /bin/echo hello\n", argv[0]);
        exit(1);
    }
    
    // Parse target time
    int target_hour, target_min;
    if (sscanf(argv[1], "%d:%d", &target_hour, &target_min) != 2) {
        printf("Invalid time format. Use HH:MM\n");
        exit(1);
    }
    
    // Daemonize
    daemonize();
    
    // Main daemon loop
    while (1) {
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);
        
        if (tm_now->tm_hour == target_hour && tm_now->tm_min == target_min) {
            // Time matched - execute command
            pid_t pid = fork();
            if (pid == 0) {
                // Child executes the command
                execvp(argv[2], &argv[2]);
                _exit(1); // exec failed
            } else if (pid > 0) {
                // Wait for command to complete
                waitpid(pid, NULL, 0);
            }
            
            // Sleep a minute to avoid re-executing in same minute
            sleep(60);
        } else {
            // Check every 30 seconds
            sleep(30);
        }
    }
    
    return 0;
}
/*
 * Output
 * Command: gcc 30.c -o 30 && ./30
 */

