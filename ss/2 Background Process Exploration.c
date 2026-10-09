// 2. Background Process Exploration: Develop a simple program to execute indefinitely
// in the background. Traverse the /proc directory and extract relevant process
// information from the corresponding proc directories.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

void print_process_info(const char *pid) {
    char path[256];
    char line[512];
    FILE *fp;
    
    // Read /proc/pid/status
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    fp = fopen(path, "r");
    if (fp) {
        printf("=== Process %s ===\n", pid);
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Name:", 5) == 0 ||
                strncmp(line, "State:", 6) == 0 ||
                strncmp(line, "Pid:", 4) == 0 ||
                strncmp(line, "PPid:", 5) == 0 ||
                strncmp(line, "Uid:", 4) == 0 ||
                strncmp(line, "Gid:", 4) == 0 ||
                strncmp(line, "VmSize:", 7) == 0 ||
                strncmp(line, "VmRSS:", 6) == 0 ||
                strncmp(line, "Threads:", 8) == 0) {
                printf("%s", line);
            }
        }
        fclose(fp);
    }

    // Read /proc/pid/cmdline
    snprintf(path, sizeof(path), "/proc/%s/cmdline", pid);
    fp = fopen(path, "r");
    if (fp) {
        printf("Cmdline: ");
        int c;
        while ((c = fgetc(fp)) != EOF) {
            if (c == 0) printf(" ");
            else putchar(c);
        }
        printf("\n");
        fclose(fp);
    }
    printf("\n");
}

int is_numeric(const char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        // Child process - run indefinitely in background
        printf("Child process started with PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        fflush(stdout);
        
        // Run indefinitely
        while (1) {
            sleep(10);
        }
    } else {
        // Parent process - traverse /proc and extract info
        printf("Parent process (PID: %d) exploring /proc directory\n", getpid());
        printf("Child process PID: %d\n\n", pid);
        
        DIR *dir = opendir("/proc");
        if (!dir) {
            perror("opendir");
            return 1;
        }
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (is_numeric(entry->d_name)) {
                print_process_info(entry->d_name);
            }
        }
        
        closedir(dir);
        
        // Wait for child (though it runs indefinitely)
        // In practice, you'd kill the child after exploration
        sleep(2);
        kill(pid, SIGTERM);
        wait(NULL);
        printf("Child process terminated.\n");
    }
    
    return 0;
}

/* Commands to run:
   gcc "2 Background Process Exploration.c" -o 2_bg_process
   ./2_bg_process
   
   # To run in background and inspect:
   ./2_bg_process &
   jobs
   ps aux | grep 2_bg_process
   ls -l /proc/<PID>/fd
*/