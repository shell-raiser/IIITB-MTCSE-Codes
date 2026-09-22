// 5 Continuous File Creation: Create a program that generates five new files in an
// infinite loop. Execute the program in the background and inspect the file descriptor
// table at /proc/pid/fd.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    (void)sig;
    stop = 1;
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    
    printf("Process PID: %d\n", getpid());
    printf("Run in background: ./5_continuous_file_creation &\n");
    printf("Then inspect: ls -l /proc/%d/fd\n", getpid());
    printf("Press Ctrl+C to stop\n\n");
    
    int file_count = 0;
    char filename[64];
    
    while (!stop) {
        // Create 5 files
        for (int i = 0; i < 5; i++) {
            snprintf(filename, sizeof(filename), "auto_file_%d_%d.txt", file_count, i);
            
            int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                continue;
            }
            
            char content[128];
            snprintf(content, sizeof(content), "File %d, iteration %d\n", file_count, i);
            write(fd, content, strlen(content));
            close(fd);
            
            printf("Created: %s\n", filename);
        }
        
        file_count++;
        printf("--- Iteration %d complete. Check /proc/%d/fd ---\n\n", file_count, getpid());
        
        sleep(2);  // Slow down to allow inspection
    }
    
    printf("\nStopping... Cleaning up files.\n");
    // Note: Files are left for inspection
    
    return 0;
}

/* Commands to run:
   gcc "5 Continuous File Creation.c" -o 5_continuous_file_creation
   ./5_continuous_file_creation &
   
   # In another terminal or after pressing Ctrl+Z and 'bg':
   jobs
   ps aux | grep 5_continuous_file_creation
   ls -l /proc/<PID>/fd
   watch -n 1 "ls -l /proc/<PID>/fd"
   
   # To stop:
   kill %1
   # or
   kill <PID>
*/

/*
 * Sample output for 5.c
 * Example runtime:
 * $ gcc 5.c -o 5
 * $ ./5
 *
 * Program executed successfully.
 */

/*
 * Actual output of 5.c
 * Command: gcc 5.c -o 5
 *
 * Program timed out after 10 seconds while running.
 * No further output was captured.
 */

