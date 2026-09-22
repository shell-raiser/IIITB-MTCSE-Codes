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

/*
 * Sample output for 2.c
 * Example runtime:
 * $ gcc 2.c -o 2
 * $ ./2
 *
 * Program executed successfully.
 */

/*
 * Actual output of 2.c
 * Command: gcc 2.c -o 2
 *
 * Child process started with PID: 48307
 * Parent PID: 48305
 * Parent process (PID: 48305) exploring /proc directory
 * Child process PID: 48307
 *
 * === Process 1 ===
 * Name:	systemd
 * State:	S (sleeping)
 * Pid:	1
 * PPid:	0
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   21480 kB
 * VmRSS:	    8316 kB
 * Threads:	1
 * Cmdline: /sbin/init splash 
 *
 * === Process 2 ===
 * Name:	kthreadd
 * State:	S (sleeping)
 * Pid:	2
 * PPid:	0
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 3 ===
 * Name:	pool_workqueue_release
 * State:	S (sleeping)
 * Pid:	3
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 4 ===
 * Name:	kworker/R-rcu_gp
 * State:	I (idle)
 * Pid:	4
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 5 ===
 * Name:	kworker/R-sync_wq
 * State:	I (idle)
 * Pid:	5
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 6 ===
 * Name:	kworker/R-kvfree_rcu_reclaim
 * State:	I (idle)
 * Pid:	6
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 7 ===
 * Name:	kworker/R-slub_flushwq
 * State:	I (idle)
 * Pid:	7
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 8 ===
 * Name:	kworker/R-netns
 * State:	I (idle)
 * Pid:	8
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 11 ===
 * Name:	kworker/0:0H-kblockd
 * State:	I (idle)
 * Pid:	11
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 14 ===
 * Name:	kworker/R-mm_percpu_wq
 * State:	I (idle)
 * Pid:	14
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 15 ===
 * Name:	ksoftirqd/0
 * State:	S (sleeping)
 * Pid:	15
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 16 ===
 * Name:	rcu_preempt
 * State:	I (idle)
 * Pid:	16
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 17 ===
 * Name:	rcub/0
 * State:	S (sleeping)
 * Pid:	17
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 18 ===
 * Name:	rcu_exp_par_gp_kthread_worker/0
 * State:	S (sleeping)
 * Pid:	18
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 19 ===
 * Name:	rcu_exp_gp_kthread_worker
 * State:	S (sleeping)
 * Pid:	19
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 20 ===
 * Name:	migration/0
 * State:	S (sleeping)
 * Pid:	20
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 21 ===
 * Name:	kprobe-optimizer
 * State:	S (sleeping)
 * Pid:	21
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 22 ===
 * Name:	idle_inject/0
 * State:	S (sleeping)
 * Pid:	22
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 23 ===
 * Name:	cpuhp/0
 * State:	S (sleeping)
 * Pid:	23
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 24 ===
 * Name:	cpuhp/1
 * State:	S (sleeping)
 * Pid:	24
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 25 ===
 * Name:	idle_inject/1
 * State:	S (sleeping)
 * Pid:	25
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 26 ===
 * Name:	migration/1
 * State:	S (sleeping)
 * Pid:	26
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 27 ===
 * Name:	ksoftirqd/1
 * State:	S (sleeping)
 * Pid:	27
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 29 ===
 * Name:	kworker/1:0H-kblockd
 * State:	I (idle)
 * Pid:	29
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 30 ===
 * Name:	cpuhp/2
 * State:	S (sleeping)
 * Pid:	30
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 31 ===
 * Name:	idle_inject/2
 * State:	S (sleeping)
 * Pid:	31
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 32 ===
 * Name:	migration/2
 * State:	S (sleeping)
 * Pid:	32
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 33 ===
 * Name:	ksoftirqd/2
 * State:	S (sleeping)
 * Pid:	33
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 35 ===
 * Name:	kworker/2:0H-kblockd
 * State:	I (idle)
 * Pid:	35
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 36 ===
 * Name:	cpuhp/3
 * State:	S (sleeping)
 * Pid:	36
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 37 ===
 * Name:	idle_inject/3
 * State:	S (sleeping)
 * Pid:	37
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 38 ===
 * Name:	migration/3
 * State:	S (sleeping)
 * Pid:	38
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 39 ===
 * Name:	ksoftirqd/3
 * State:	S (sleeping)
 * Pid:	39
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 41 ===
 * Name:	kworker/3:0H-kblockd
 * State:	I (idle)
 * Pid:	41
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42 ===
 * Name:	cpuhp/4
 * State:	S (sleeping)
 * Pid:	42
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43 ===
 * Name:	idle_inject/4
 * State:	S (sleeping)
 * Pid:	43
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 44 ===
 * Name:	migration/4
 * State:	S (sleeping)
 * Pid:	44
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 45 ===
 * Name:	ksoftirqd/4
 * State:	S (sleeping)
 * Pid:	45
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47 ===
 * Name:	kworker/4:0H-kblockd
 * State:	I (idle)
 * Pid:	47
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48 ===
 * Name:	cpuhp/5
 * State:	S (sleeping)
 * Pid:	48
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 49 ===
 * Name:	idle_inject/5
 * State:	S (sleeping)
 * Pid:	49
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 50 ===
 * Name:	migration/5
 * State:	S (sleeping)
 * Pid:	50
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 51 ===
 * Name:	ksoftirqd/5
 * State:	S (sleeping)
 * Pid:	51
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 53 ===
 * Name:	kworker/5:0H-kblockd
 * State:	I (idle)
 * Pid:	53
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 54 ===
 * Name:	cpuhp/6
 * State:	S (sleeping)
 * Pid:	54
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 55 ===
 * Name:	idle_inject/6
 * State:	S (sleeping)
 * Pid:	55
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 56 ===
 * Name:	migration/6
 * State:	S (sleeping)
 * Pid:	56
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 57 ===
 * Name:	ksoftirqd/6
 * State:	S (sleeping)
 * Pid:	57
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 59 ===
 * Name:	kworker/6:0H-kblockd
 * State:	I (idle)
 * Pid:	59
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 60 ===
 * Name:	cpuhp/7
 * State:	S (sleeping)
 * Pid:	60
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 61 ===
 * Name:	idle_inject/7
 * State:	S (sleeping)
 * Pid:	61
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 62 ===
 * Name:	migration/7
 * State:	S (sleeping)
 * Pid:	62
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 63 ===
 * Name:	ksoftirqd/7
 * State:	S (sleeping)
 * Pid:	63
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 65 ===
 * Name:	kworker/7:0H-kblockd
 * State:	I (idle)
 * Pid:	65
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 68 ===
 * Name:	kdevtmpfs
 * State:	S (sleeping)
 * Pid:	68
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 69 ===
 * Name:	kworker/R-inet_frag_wq
 * State:	I (idle)
 * Pid:	69
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 70 ===
 * Name:	rcu_tasks_kthread
 * State:	I (idle)
 * Pid:	70
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 71 ===
 * Name:	rcu_tasks_rude_kthread
 * State:	I (idle)
 * Pid:	71
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 72 ===
 * Name:	kauditd
 * State:	S (sleeping)
 * Pid:	72
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 73 ===
 * Name:	khungtaskd
 * State:	S (sleeping)
 * Pid:	73
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 74 ===
 * Name:	oom_reaper
 * State:	S (sleeping)
 * Pid:	74
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 77 ===
 * Name:	kworker/R-writeback
 * State:	I (idle)
 * Pid:	77
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 78 ===
 * Name:	kcompactd0
 * State:	S (sleeping)
 * Pid:	78
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 79 ===
 * Name:	ksmd
 * State:	S (sleeping)
 * Pid:	79
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 80 ===
 * Name:	khugepaged
 * State:	S (sleeping)
 * Pid:	80
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 81 ===
 * Name:	kworker/R-kblockd
 * State:	I (idle)
 * Pid:	81
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 82 ===
 * Name:	kworker/R-blkcg_punt_bio
 * State:	I (idle)
 * Pid:	82
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 83 ===
 * Name:	kworker/R-kintegrityd
 * State:	I (idle)
 * Pid:	83
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 84 ===
 * Name:	irq/9-acpi
 * State:	S (sleeping)
 * Pid:	84
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 87 ===
 * Name:	kworker/R-tpm_dev_wq
 * State:	I (idle)
 * Pid:	87
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 88 ===
 * Name:	kworker/R-ata_sff
 * State:	I (idle)
 * Pid:	88
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 89 ===
 * Name:	kworker/R-edac-poller
 * State:	I (idle)
 * Pid:	89
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 90 ===
 * Name:	kworker/R-devfreq_wq
 * State:	I (idle)
 * Pid:	90
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 91 ===
 * Name:	watchdogd
 * State:	S (sleeping)
 * Pid:	91
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 92 ===
 * Name:	kworker/R-quota_events_unbound
 * State:	I (idle)
 * Pid:	92
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 93 ===
 * Name:	irq/25-AMD-Vi0-Evt
 * State:	S (sleeping)
 * Pid:	93
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 94 ===
 * Name:	irq/26-AMD-Vi0-PPR
 * State:	S (sleeping)
 * Pid:	94
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 95 ===
 * Name:	irq/27-AMD-Vi0-GA
 * State:	S (sleeping)
 * Pid:	95
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 96 ===
 * Name:	kswapd0
 * State:	S (sleeping)
 * Pid:	96
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 98 ===
 * Name:	kworker/R-kthrotld
 * State:	I (idle)
 * Pid:	98
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 100 ===
 * Name:	irq/36-pciehp
 * State:	S (sleeping)
 * Pid:	100
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 101 ===
 * Name:	kworker/R-acpi_thermal_pm
 * State:	I (idle)
 * Pid:	101
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 106 ===
 * Name:	kworker/R-mld
 * State:	I (idle)
 * Pid:	106
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 107 ===
 * Name:	kworker/R-ipv6_addrconf
 * State:	I (idle)
 * Pid:	107
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 108 ===
 * Name:	kworker/R-kstrp
 * State:	I (idle)
 * Pid:	108
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 115 ===
 * Name:	kworker/R-zswap-shrink
 * State:	I (idle)
 * Pid:	115
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 121 ===
 * Name:	irq/28-ACPI:Event
 * State:	S (sleeping)
 * Pid:	121
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 122 ===
 * Name:	irq/29-ACPI:Event
 * State:	S (sleeping)
 * Pid:	122
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 123 ===
 * Name:	irq/30-ACPI:Event
 * State:	S (sleeping)
 * Pid:	123
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 124 ===
 * Name:	irq/31-ACPI:Event
 * State:	S (sleeping)
 * Pid:	124
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 125 ===
 * Name:	irq/32-ACPI:Event
 * State:	S (sleeping)
 * Pid:	125
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 126 ===
 * Name:	irq/33-ACPI:Event
 * State:	S (sleeping)
 * Pid:	126
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 127 ===
 * Name:	irq/34-ACPI:Event
 * State:	S (sleeping)
 * Pid:	127
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 128 ===
 * Name:	irq/35-ACPI:Event
 * State:	S (sleeping)
 * Pid:	128
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 204 ===
 * Name:	kworker/R-nvme-wq
 * State:	I (idle)
 * Pid:	204
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 205 ===
 * Name:	kworker/R-nvme-reset-wq
 * State:	I (idle)
 * Pid:	205
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 206 ===
 * Name:	kworker/R-nvme-delete-wq
 * State:	I (idle)
 * Pid:	206
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 207 ===
 * Name:	kworker/R-nvme-auth-wq
 * State:	I (idle)
 * Pid:	207
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 211 ===
 * Name:	kworker/R-amdgpu-reset-dev
 * State:	I (idle)
 * Pid:	211
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 212 ===
 * Name:	kworker/R-ttm
 * State:	I (idle)
 * Pid:	212
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 213 ===
 * Name:	kworker/R-amdgpu_dm_hpd_rx_offload_wq
 * State:	I (idle)
 * Pid:	213
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 214 ===
 * Name:	kworker/R-amdgpu_dm_hpd_rx_offload_wq
 * State:	I (idle)
 * Pid:	214
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 215 ===
 * Name:	kworker/R-dm_vblank_control_workqueue
 * State:	I (idle)
 * Pid:	215
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 216 ===
 * Name:	card1-crtc0
 * State:	S (sleeping)
 * Pid:	216
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 217 ===
 * Name:	card1-crtc1
 * State:	S (sleeping)
 * Pid:	217
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 218 ===
 * Name:	card1-crtc2
 * State:	S (sleeping)
 * Pid:	218
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 219 ===
 * Name:	card1-crtc3
 * State:	S (sleeping)
 * Pid:	219
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 220 ===
 * Name:	kworker/R-gfx
 * State:	I (idle)
 * Pid:	220
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 221 ===
 * Name:	kworker/R-comp_1.0.0
 * State:	I (idle)
 * Pid:	221
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 222 ===
 * Name:	kworker/R-comp_1.1.0
 * State:	I (idle)
 * Pid:	222
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 223 ===
 * Name:	kworker/R-comp_1.2.0
 * State:	I (idle)
 * Pid:	223
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 224 ===
 * Name:	kworker/R-comp_1.3.0
 * State:	I (idle)
 * Pid:	224
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 225 ===
 * Name:	kworker/R-comp_1.0.1
 * State:	I (idle)
 * Pid:	225
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 226 ===
 * Name:	kworker/R-comp_1.1.1
 * State:	I (idle)
 * Pid:	226
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 227 ===
 * Name:	kworker/R-comp_1.2.1
 * State:	I (idle)
 * Pid:	227
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 228 ===
 * Name:	kworker/R-comp_1.3.1
 * State:	I (idle)
 * Pid:	228
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 229 ===
 * Name:	kworker/R-sdma0
 * State:	I (idle)
 * Pid:	229
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 230 ===
 * Name:	kworker/R-vcn_dec
 * State:	I (idle)
 * Pid:	230
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 231 ===
 * Name:	kworker/R-vcn_enc0
 * State:	I (idle)
 * Pid:	231
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 232 ===
 * Name:	kworker/R-vcn_enc1
 * State:	I (idle)
 * Pid:	232
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 233 ===
 * Name:	kworker/R-jpeg_dec
 * State:	I (idle)
 * Pid:	233
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 266 ===
 * Name:	kworker/R-kdmflush/253:0
 * State:	I (idle)
 * Pid:	266
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 267 ===
 * Name:	kworker/R-kcryptd_io-253:0-1
 * State:	I (idle)
 * Pid:	267
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 268 ===
 * Name:	kworker/R-kcryptd-253:0-1
 * State:	I (idle)
 * Pid:	268
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 269 ===
 * Name:	dmcrypt_write/253:0
 * State:	S (sleeping)
 * Pid:	269
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 292 ===
 * Name:	kworker/R-btrfs-worker
 * State:	I (idle)
 * Pid:	292
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 293 ===
 * Name:	kworker/R-btrfs-delalloc
 * State:	I (idle)
 * Pid:	293
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 294 ===
 * Name:	kworker/R-btrfs-flush_delalloc
 * State:	I (idle)
 * Pid:	294
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 295 ===
 * Name:	kworker/R-btrfs-cache
 * State:	I (idle)
 * Pid:	295
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 296 ===
 * Name:	kworker/R-btrfs-fixup
 * State:	I (idle)
 * Pid:	296
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 297 ===
 * Name:	kworker/R-btrfs-endio
 * State:	I (idle)
 * Pid:	297
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 298 ===
 * Name:	kworker/R-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	298
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 299 ===
 * Name:	kworker/R-btrfs-rmw
 * State:	I (idle)
 * Pid:	299
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 300 ===
 * Name:	kworker/R-btrfs-endio-write
 * State:	I (idle)
 * Pid:	300
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 301 ===
 * Name:	kworker/R-btrfs-freespace-write
 * State:	I (idle)
 * Pid:	301
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 302 ===
 * Name:	kworker/R-btrfs-delayed-meta
 * State:	I (idle)
 * Pid:	302
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 303 ===
 * Name:	kworker/R-btrfs-qgroup-rescan
 * State:	I (idle)
 * Pid:	303
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 304 ===
 * Name:	btrfs-cleaner
 * State:	S (sleeping)
 * Pid:	304
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 305 ===
 * Name:	btrfs-transaction
 * State:	S (sleeping)
 * Pid:	305
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 315 ===
 * Name:	kworker/0:1H-kblockd
 * State:	I (idle)
 * Pid:	315
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 318 ===
 * Name:	kworker/5:1H-kblockd
 * State:	I (idle)
 * Pid:	318
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 319 ===
 * Name:	kworker/7:1H-kblockd
 * State:	I (idle)
 * Pid:	319
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 320 ===
 * Name:	kworker/6:1H-kblockd
 * State:	I (idle)
 * Pid:	320
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 321 ===
 * Name:	kworker/3:1H-kblockd
 * State:	I (idle)
 * Pid:	321
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 324 ===
 * Name:	kworker/1:1H-kblockd
 * State:	I (idle)
 * Pid:	324
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 326 ===
 * Name:	kworker/2:1H-kblockd
 * State:	I (idle)
 * Pid:	326
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 356 ===
 * Name:	kworker/4:1H-kblockd
 * State:	I (idle)
 * Pid:	356
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 365 ===
 * Name:	systemd-journal
 * State:	S (sleeping)
 * Pid:	365
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   86304 kB
 * VmRSS:	    9204 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd-journald 
 *
 * === Process 381 ===
 * Name:	systemd-userdbd
 * State:	S (sleeping)
 * Pid:	381
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    9124 kB
 * VmRSS:	    2084 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd-userdbd 
 *
 * === Process 404 ===
 * Name:	psimon
 * State:	S (sleeping)
 * Pid:	404
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 410 ===
 * Name:	systemd-timesyn
 * State:	S (sleeping)
 * Pid:	410
 * PPid:	1
 * Uid:	975	975	975	975
 * Gid:	975	975	975	975
 * VmSize:	   85440 kB
 * VmRSS:	    3108 kB
 * Threads:	2
 * Cmdline: /usr/lib/systemd/systemd-timesyncd 
 *
 * === Process 425 ===
 * Name:	systemd-udevd
 * State:	S (sleeping)
 * Pid:	425
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   43920 kB
 * VmRSS:	    4396 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd-udevd 
 *
 * === Process 426 ===
 * Name:	psimon
 * State:	S (sleeping)
 * Pid:	426
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 508 ===
 * Name:	irq/82-ELAN1301:00
 * State:	S (sleeping)
 * Pid:	508
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 536 ===
 * Name:	kworker/R-cfg80211
 * State:	I (idle)
 * Pid:	536
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 575 ===
 * Name:	irq/84-iwlwifi:default_queue
 * State:	S (sleeping)
 * Pid:	575
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 576 ===
 * Name:	irq/85-iwlwifi:queue_1
 * State:	S (sleeping)
 * Pid:	576
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 577 ===
 * Name:	irq/86-iwlwifi:queue_2
 * State:	S (sleeping)
 * Pid:	577
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 578 ===
 * Name:	irq/87-iwlwifi:queue_3
 * State:	S (sleeping)
 * Pid:	578
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 579 ===
 * Name:	irq/88-iwlwifi:queue_4
 * State:	S (sleeping)
 * Pid:	579
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 580 ===
 * Name:	irq/89-iwlwifi:queue_5
 * State:	S (sleeping)
 * Pid:	580
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 581 ===
 * Name:	irq/90-iwlwifi:queue_6
 * State:	S (sleeping)
 * Pid:	581
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 582 ===
 * Name:	irq/91-iwlwifi:queue_7
 * State:	S (sleeping)
 * Pid:	582
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 583 ===
 * Name:	irq/92-iwlwifi:queue_8
 * State:	S (sleeping)
 * Pid:	583
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 584 ===
 * Name:	irq/93-iwlwifi:exception
 * State:	S (sleeping)
 * Pid:	584
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 693 ===
 * Name:	kworker/R-led_workqueue
 * State:	I (idle)
 * Pid:	693
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 869 ===
 * Name:	kworker/R-kdmflush/253:1
 * State:	I (idle)
 * Pid:	869
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 870 ===
 * Name:	kworker/R-kcryptd_io-253:1-2
 * State:	I (idle)
 * Pid:	870
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 871 ===
 * Name:	kworker/R-kcryptd-253:1-2
 * State:	I (idle)
 * Pid:	871
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 872 ===
 * Name:	dmcrypt_write/253:1
 * State:	S (sleeping)
 * Pid:	872
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 879 ===
 * Name:	kworker/R-btrfs-worker
 * State:	I (idle)
 * Pid:	879
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 880 ===
 * Name:	kworker/R-btrfs-delalloc
 * State:	I (idle)
 * Pid:	880
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 881 ===
 * Name:	kworker/R-btrfs-flush_delalloc
 * State:	I (idle)
 * Pid:	881
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 882 ===
 * Name:	kworker/R-btrfs-cache
 * State:	I (idle)
 * Pid:	882
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 883 ===
 * Name:	kworker/R-btrfs-fixup
 * State:	I (idle)
 * Pid:	883
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 884 ===
 * Name:	kworker/R-btrfs-endio
 * State:	I (idle)
 * Pid:	884
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 885 ===
 * Name:	kworker/R-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	885
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 886 ===
 * Name:	kworker/R-btrfs-rmw
 * State:	I (idle)
 * Pid:	886
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 887 ===
 * Name:	kworker/R-btrfs-endio-write
 * State:	I (idle)
 * Pid:	887
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 888 ===
 * Name:	kworker/R-btrfs-freespace-write
 * State:	I (idle)
 * Pid:	888
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 889 ===
 * Name:	kworker/R-btrfs-delayed-meta
 * State:	I (idle)
 * Pid:	889
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 890 ===
 * Name:	kworker/R-btrfs-qgroup-rescan
 * State:	I (idle)
 * Pid:	890
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 892 ===
 * Name:	btrfs-cleaner
 * State:	S (sleeping)
 * Pid:	892
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 893 ===
 * Name:	btrfs-transaction
 * State:	S (sleeping)
 * Pid:	893
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 917 ===
 * Name:	dbus-broker-lau
 * State:	S (sleeping)
 * Pid:	917
 * PPid:	1
 * Uid:	81	81	81	81
 * Gid:	81	81	81	81
 * VmSize:	    8240 kB
 * VmRSS:	    2052 kB
 * Threads:	1
 * Cmdline: /usr/bin/dbus-broker-launch --scope system --audit 
 *
 * === Process 918 ===
 * Name:	dbus-broker
 * State:	S (sleeping)
 * Pid:	918
 * PPid:	917
 * Uid:	81	81	81	81
 * Gid:	81	81	81	81
 * VmSize:	    8360 kB
 * VmRSS:	    4512 kB
 * Threads:	1
 * Cmdline: dbus-broker --log 10 --controller 9 --machine-id a2d1c3eab83e458f882a3a2d293e163e --max-bytes 536870912 --max-fds 4096 --max-matches 131072 --audit 
 *
 * === Process 920 ===
 * Name:	avahi-daemon
 * State:	S (sleeping)
 * Pid:	920
 * PPid:	1
 * Uid:	968	968	968	968
 * Gid:	968	968	968	968
 * VmSize:	    6264 kB
 * VmRSS:	    2512 kB
 * Threads:	1
 * Cmdline: avahi-daemon: running [manjarolinux.local] 
 *
 * === Process 921 ===
 * Name:	avahi-dnsconfd
 * State:	S (sleeping)
 * Pid:	921
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    3092 kB
 * VmRSS:	    1580 kB
 * Threads:	1
 * Cmdline: /usr/bin/avahi-dnsconfd -s 
 *
 * === Process 922 ===
 * Name:	bluetoothd
 * State:	S (sleeping)
 * Pid:	922
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   11804 kB
 * VmRSS:	    3792 kB
 * Threads:	1
 * Cmdline: /usr/lib/bluetooth/bluetoothd 
 *
 * === Process 923 ===
 * Name:	firewalld
 * State:	S (sleeping)
 * Pid:	923
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  150072 kB
 * VmRSS:	    8956 kB
 * Threads:	2
 * Cmdline: /usr/bin/python /usr/bin/firewalld --nofork --nopid 
 *
 * === Process 925 ===
 * Name:	NetworkManager
 * State:	S (sleeping)
 * Pid:	925
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  431080 kB
 * VmRSS:	   12412 kB
 * Threads:	4
 * Cmdline: /usr/bin/NetworkManager --no-daemon 
 *
 * === Process 926 ===
 * Name:	polkitd
 * State:	S (sleeping)
 * Pid:	926
 * PPid:	1
 * Uid:	102	102	102	102
 * Gid:	102	102	102	102
 * VmSize:	  389052 kB
 * VmRSS:	    9840 kB
 * Threads:	4
 * Cmdline: /usr/lib/polkit-1/polkitd --no-debug --log-level=notice 
 *
 * === Process 927 ===
 * Name:	systemd-logind
 * State:	S (sleeping)
 * Pid:	927
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   10984 kB
 * VmRSS:	    4932 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd-logind 
 *
 * === Process 928 ===
 * Name:	systemd-machine
 * State:	S (sleeping)
 * Pid:	928
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    9908 kB
 * VmRSS:	    3844 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd-machined 
 *
 * === Process 945 ===
 * Name:	avahi-daemon
 * State:	S (sleeping)
 * Pid:	945
 * PPid:	920
 * Uid:	968	968	968	968
 * Gid:	968	968	968	968
 * VmSize:	    6092 kB
 * VmRSS:	    1012 kB
 * Threads:	1
 * Cmdline: avahi-daemon: chroot helper 
 *
 * === Process 982 ===
 * Name:	ModemManager
 * State:	S (sleeping)
 * Pid:	982
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  391252 kB
 * VmRSS:	    4536 kB
 * Threads:	4
 * Cmdline: /usr/bin/ModemManager 
 *
 * === Process 1000 ===
 * Name:	wpa_supplicant
 * State:	S (sleeping)
 * Pid:	1000
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   21356 kB
 * VmRSS:	    3780 kB
 * Threads:	1
 * Cmdline: /usr/bin/wpa_supplicant -u -s -O /run/wpa_supplicant 
 *
 * === Process 1007 ===
 * Name:	tailscaled
 * State:	S (sleeping)
 * Pid:	1007
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	 2000308 kB
 * VmRSS:	   54820 kB
 * Threads:	15
 * Cmdline: /usr/sbin/tailscaled --state=/var/lib/tailscale/tailscaled.state --socket=/run/tailscale/tailscaled.sock --port=41641 
 *
 * === Process 1016 ===
 * Name:	cupsd
 * State:	S (sleeping)
 * Pid:	1016
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   22888 kB
 * VmRSS:	    3868 kB
 * Threads:	1
 * Cmdline: /usr/bin/cupsd -l 
 *
 * === Process 1017 ===
 * Name:	libvirtd
 * State:	S (sleeping)
 * Pid:	1017
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  743072 kB
 * VmRSS:	    9844 kB
 * Threads:	23
 * Cmdline: /usr/bin/libvirtd --timeout 120 
 *
 * === Process 1018 ===
 * Name:	sshd
 * State:	S (sleeping)
 * Pid:	1018
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   12884 kB
 * VmRSS:	    1728 kB
 * Threads:	1
 * Cmdline: sshd: /usr/bin/sshd -D [listener] 0 of 10-100 startups 
 *
 * === Process 1029 ===
 * Name:	gdm
 * State:	S (sleeping)
 * Pid:	1029
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  383920 kB
 * VmRSS:	    5944 kB
 * Threads:	5
 * Cmdline: /usr/bin/gdm 
 *
 * === Process 1035 ===
 * Name:	accounts-daemon
 * State:	S (sleeping)
 * Pid:	1035
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  309860 kB
 * VmRSS:	    6592 kB
 * Threads:	4
 * Cmdline: /usr/lib/accounts-daemon 
 *
 * === Process 1074 ===
 * Name:	psimon
 * State:	S (sleeping)
 * Pid:	1074
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 1137 ===
 * Name:	rtkit-daemon
 * State:	S (sleeping)
 * Pid:	1137
 * PPid:	1
 * Uid:	133	133	133	133
 * Gid:	133	133	133	133
 * VmSize:	   21104 kB
 * VmRSS:	    1936 kB
 * Threads:	3
 * Cmdline: /usr/lib/rtkit-daemon 
 *
 * === Process 1170 ===
 * Name:	colord
 * State:	S (sleeping)
 * Pid:	1170
 * PPid:	1
 * Uid:	967	967	967	967
 * Gid:	967	967	967	967
 * VmSize:	  317968 kB
 * VmRSS:	    5496 kB
 * Threads:	4
 * Cmdline: /usr/lib/colord 
 *
 * === Process 1243 ===
 * Name:	upowerd
 * State:	S (sleeping)
 * Pid:	1243
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  320812 kB
 * VmRSS:	    7400 kB
 * Threads:	4
 * Cmdline: /usr/lib/upowerd 
 *
 * === Process 1875 ===
 * Name:	krfcommd
 * State:	S (sleeping)
 * Pid:	1875
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 1896 ===
 * Name:	dnsmasq
 * State:	S (sleeping)
 * Pid:	1896
 * PPid:	1
 * Uid:	65534	65534	65534	65534
 * Gid:	65534	65534	65534	65534
 * VmSize:	   10868 kB
 * VmRSS:	    1636 kB
 * Threads:	1
 * Cmdline: /usr/bin/dnsmasq --conf-file=/var/lib/libvirt/dnsmasq/default.conf --leasefile-ro --dhcp-script=/usr/lib/libvirt/libvirt_leaseshelper 
 *
 * === Process 1897 ===
 * Name:	dnsmasq
 * State:	S (sleeping)
 * Pid:	1897
 * PPid:	1896
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   10840 kB
 * VmRSS:	    1392 kB
 * Threads:	1
 * Cmdline: /usr/bin/dnsmasq --conf-file=/var/lib/libvirt/dnsmasq/default.conf --leasefile-ro --dhcp-script=/usr/lib/libvirt/libvirt_leaseshelper 
 *
 * === Process 2048 ===
 * Name:	virtlogd
 * State:	S (sleeping)
 * Pid:	2048
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	   82528 kB
 * VmRSS:	    6140 kB
 * Threads:	1
 * Cmdline: /usr/bin/virtlogd 
 *
 * === Process 2359 ===
 * Name:	rclone
 * State:	S (sleeping)
 * Pid:	2359
 * PPid:	1
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 1928764 kB
 * VmRSS:	   19068 kB
 * Threads:	13
 * Cmdline: /usr/bin/rclone mount shaileshuniv: /mnt/shaileshuniv --config /home/kss/.config/rclone/rclone.conf --vfs-cache-mode full --vfs-cache-max-size 10G --vfs-read-chunk-size 32M --vfs-read-chunk-size-limit off --buffer-size 64M --dir-cache-time 1000h --poll-interval 15s --attr-timeout 10m --drive-chunk-size 64M --uid 1000 --gid 1000 --allow-other --umask 022 
 *
 * === Process 2378 ===
 * Name:	rclone
 * State:	S (sleeping)
 * Pid:	2378
 * PPid:	1
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 2461320 kB
 * VmRSS:	   24552 kB
 * Threads:	14
 * Cmdline: /usr/bin/rclone mount shaileshunivpg: /mnt/shaileshunivpg --config /home/kss/.config/rclone/rclone.conf --vfs-cache-mode full --vfs-cache-max-size 10G --vfs-read-chunk-size 32M --vfs-read-chunk-size-limit off --buffer-size 64M --dir-cache-time 1000h --poll-interval 15s --attr-timeout 10m --drive-chunk-size 64M --uid 1000 --gid 1000 --allow-other --umask 022 
 *
 * === Process 2955 ===
 * Name:	udisksd
 * State:	S (sleeping)
 * Pid:	2955
 * PPid:	1
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	  545780 kB
 * VmRSS:	    5172 kB
 * Threads:	7
 * Cmdline: /usr/lib/udisks2/udisksd 
 *
 * === Process 10113 ===
 * Name:	kworker/u35:1-hci0
 * State:	I (idle)
 * Pid:	10113
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 14319 ===
 * Name:	gdm-session-wor
 * State:	S (sleeping)
 * Pid:	14319
 * PPid:	1029
 * Uid:	0	0	0	0
 * Gid:	1000	1000	1000	1000
 * VmSize:	  390236 kB
 * VmRSS:	    7268 kB
 * Threads:	4
 * Cmdline: gdm-session-worker [pam/gdm-password] 
 *
 * === Process 14481 ===
 * Name:	systemd
 * State:	S (sleeping)
 * Pid:	14481
 * PPid:	1
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   19812 kB
 * VmRSS:	    7792 kB
 * Threads:	1
 * Cmdline: /usr/lib/systemd/systemd --user 
 *
 * === Process 14483 ===
 * Name:	(sd-pam)
 * State:	S (sleeping)
 * Pid:	14483
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   19752 kB
 * VmRSS:	    1384 kB
 * Threads:	1
 * Cmdline: (sd-pam) 
 *
 * === Process 14492 ===
 * Name:	gnome-keyring-d
 * State:	S (sleeping)
 * Pid:	14492
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  182520 kB
 * VmRSS:	    7272 kB
 * Threads:	5
 * Cmdline: /usr/bin/gnome-keyring-daemon --foreground --components=pkcs11,secrets --control-directory=/run/user/1000/keyring 
 *
 * === Process 14497 ===
 * Name:	dbus-broker-lau
 * State:	S (sleeping)
 * Pid:	14497
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    7768 kB
 * VmRSS:	    2152 kB
 * Threads:	1
 * Cmdline: /usr/bin/dbus-broker-launch --scope user 
 *
 * === Process 14498 ===
 * Name:	dbus-broker
 * State:	S (sleeping)
 * Pid:	14498
 * PPid:	14497
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    7532 kB
 * VmRSS:	    3948 kB
 * Threads:	1
 * Cmdline: dbus-broker --log 11 --controller 10 --machine-id a2d1c3eab83e458f882a3a2d293e163e --max-bytes 100000000000000 --max-fds 25000000000000 --max-matches 5000000000 
 *
 * === Process 14528 ===
 * Name:	gdm-wayland-ses
 * State:	S (sleeping)
 * Pid:	14528
 * PPid:	14319
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  168596 kB
 * VmRSS:	    4776 kB
 * Threads:	4
 * Cmdline: /usr/lib/gdm-wayland-session /usr/bin/gnome-session 
 *
 * === Process 14535 ===
 * Name:	kworker/u35:2-hci0
 * State:	I (idle)
 * Pid:	14535
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 14546 ===
 * Name:	gnome-session-i
 * State:	S (sleeping)
 * Pid:	14546
 * PPid:	14528
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  212972 kB
 * VmRSS:	    2960 kB
 * Threads:	4
 * Cmdline: /usr/lib/gnome-session-init-worker gnome 
 *
 * === Process 14610 ===
 * Name:	gnome-remote-de
 * State:	S (sleeping)
 * Pid:	14610
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 1176848 kB
 * VmRSS:	   25108 kB
 * Threads:	13
 * Cmdline: /usr/lib/gnome-remote-desktop-daemon 
 *
 * === Process 14611 ===
 * Name:	gnome-session-c
 * State:	S (sleeping)
 * Pid:	14611
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   86552 kB
 * VmRSS:	    4412 kB
 * Threads:	2
 * Cmdline: /usr/lib/gnome-session-ctl --monitor 
 *
 * === Process 14615 ===
 * Name:	gnome-session-s
 * State:	S (sleeping)
 * Pid:	14615
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  586080 kB
 * VmRSS:	    6852 kB
 * Threads:	5
 * Cmdline: /usr/lib/gnome-session-service --session=gnome 
 *
 * === Process 14622 ===
 * Name:	gnome-shell
 * State:	S (sleeping)
 * Pid:	14622
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 3767044 kB
 * VmRSS:	  200640 kB
 * Threads:	36
 * Cmdline: /usr/bin/gnome-shell --mode=user 
 *
 * === Process 14635 ===
 * Name:	gvfsd
 * State:	S (sleeping)
 * Pid:	14635
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  331296 kB
 * VmRSS:	    6524 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfsd 
 *
 * === Process 14641 ===
 * Name:	gvfsd-fuse
 * State:	S (sleeping)
 * Pid:	14641
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  325000 kB
 * VmRSS:	    4768 kB
 * Threads:	6
 * Cmdline: /usr/lib/gvfsd-fuse /run/user/1000/gvfs -f 
 *
 * === Process 14682 ===
 * Name:	at-spi-bus-laun
 * State:	S (sleeping)
 * Pid:	14682
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  380768 kB
 * VmRSS:	    3356 kB
 * Threads:	5
 * Cmdline: /usr/lib/at-spi-bus-launcher 
 *
 * === Process 14688 ===
 * Name:	dbus-broker-lau
 * State:	S (sleeping)
 * Pid:	14688
 * PPid:	14682
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    7604 kB
 * VmRSS:	    2076 kB
 * Threads:	1
 * Cmdline: /usr/bin/dbus-broker-launch --config-file=/usr/share/defaults/at-spi2/accessibility.conf --scope user 
 *
 * === Process 14689 ===
 * Name:	dbus-broker
 * State:	S (sleeping)
 * Pid:	14689
 * PPid:	14688
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    4864 kB
 * VmRSS:	    1920 kB
 * Threads:	1
 * Cmdline: dbus-broker --log 10 --controller 9 --machine-id a2d1c3eab83e458f882a3a2d293e163e --max-bytes 100000000000000 --max-fds 6400000 --max-matches 5000000000 
 *
 * === Process 14690 ===
 * Name:	at-spi2-registr
 * State:	S (sleeping)
 * Pid:	14690
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  168016 kB
 * VmRSS:	    3340 kB
 * Threads:	4
 * Cmdline: /usr/lib/at-spi2-registryd --use-gnome-session 
 *
 * === Process 14698 ===
 * Name:	xdg-permission-
 * State:	S (sleeping)
 * Pid:	14698
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  307808 kB
 * VmRSS:	    5288 kB
 * Threads:	4
 * Cmdline: /usr/lib/xdg-permission-store 
 *
 * === Process 14710 ===
 * Name:	gnome-shell-cal
 * State:	S (sleeping)
 * Pid:	14710
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  621604 kB
 * VmRSS:	    5956 kB
 * Threads:	7
 * Cmdline: /usr/lib/gnome-shell-calendar-server 
 *
 * === Process 14722 ===
 * Name:	evolution-sourc
 * State:	S (sleeping)
 * Pid:	14722
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  959264 kB
 * VmRSS:	   14168 kB
 * Threads:	6
 * Cmdline: /usr/lib/evolution-source-registry 
 *
 * === Process 14724 ===
 * Name:	gvfsd-metadata
 * State:	S (sleeping)
 * Pid:	14724
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  169556 kB
 * VmRSS:	    5640 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfsd-metadata 
 *
 * === Process 14737 ===
 * Name:	gjs
 * State:	S (sleeping)
 * Pid:	14737
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 2671332 kB
 * VmRSS:	   15340 kB
 * Threads:	12
 * Cmdline: /usr/bin/gjs -m /usr/share/gnome-shell/org.gnome.Shell.Notifications 
 *
 * === Process 14743 ===
 * Name:	ibus-daemon
 * State:	S (sleeping)
 * Pid:	14743
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  386428 kB
 * VmRSS:	    6672 kB
 * Threads:	4
 * Cmdline: /usr/bin/ibus-daemon --panel disable 
 *
 * === Process 14744 ===
 * Name:	gsd-a11y-settin
 * State:	S (sleeping)
 * Pid:	14744
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  291032 kB
 * VmRSS:	    5592 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-a11y-settings 
 *
 * === Process 14745 ===
 * Name:	gsd-color
 * State:	S (sleeping)
 * Pid:	14745
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  392084 kB
 * VmRSS:	    6296 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-color 
 *
 * === Process 14748 ===
 * Name:	gsd-datetime
 * State:	S (sleeping)
 * Pid:	14748
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  438596 kB
 * VmRSS:	    6460 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-datetime 
 *
 * === Process 14750 ===
 * Name:	gsd-housekeepin
 * State:	S (sleeping)
 * Pid:	14750
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  392148 kB
 * VmRSS:	    7056 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-housekeeping 
 *
 * === Process 14751 ===
 * Name:	gsd-keyboard
 * State:	S (sleeping)
 * Pid:	14751
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  384112 kB
 * VmRSS:	    5388 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-keyboard 
 *
 * === Process 14755 ===
 * Name:	gsd-media-keys
 * State:	S (sleeping)
 * Pid:	14755
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  727468 kB
 * VmRSS:	    9420 kB
 * Threads:	6
 * Cmdline: /usr/lib/gsd-media-keys 
 *
 * === Process 14764 ===
 * Name:	gsd-power
 * State:	S (sleeping)
 * Pid:	14764
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  651504 kB
 * VmRSS:	    6984 kB
 * Threads:	6
 * Cmdline: /usr/lib/gsd-power 
 *
 * === Process 14767 ===
 * Name:	gsd-print-notif
 * State:	S (sleeping)
 * Pid:	14767
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  327888 kB
 * VmRSS:	    6908 kB
 * Threads:	4
 * Cmdline: /usr/lib/gsd-print-notifications 
 *
 * === Process 14769 ===
 * Name:	gsd-rfkill
 * State:	S (sleeping)
 * Pid:	14769
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  457612 kB
 * VmRSS:	    5684 kB
 * Threads:	4
 * Cmdline: /usr/lib/gsd-rfkill 
 *
 * === Process 14771 ===
 * Name:	gsd-screensaver
 * State:	S (sleeping)
 * Pid:	14771
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  170496 kB
 * VmRSS:	    5320 kB
 * Threads:	4
 * Cmdline: /usr/lib/gsd-screensaver-proxy 
 *
 * === Process 14780 ===
 * Name:	gsd-sharing
 * State:	S (sleeping)
 * Pid:	14780
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  462248 kB
 * VmRSS:	    7360 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-sharing 
 *
 * === Process 14786 ===
 * Name:	gsd-disk-utilit
 * State:	S (sleeping)
 * Pid:	14786
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  313752 kB
 * VmRSS:	    3616 kB
 * Threads:	4
 * Cmdline: /usr/lib/gsd-disk-utility-notify 
 *
 * === Process 14795 ===
 * Name:	gsd-smartcard
 * State:	S (sleeping)
 * Pid:	14795
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  386372 kB
 * VmRSS:	    5536 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-smartcard 
 *
 * === Process 14796 ===
 * Name:	gsd-sound
 * State:	S (sleeping)
 * Pid:	14796
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  253068 kB
 * VmRSS:	    5368 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-sound 
 *
 * === Process 14799 ===
 * Name:	gsd-usb-protect
 * State:	S (sleeping)
 * Pid:	14799
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  546952 kB
 * VmRSS:	    5680 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-usb-protection 
 *
 * === Process 14800 ===
 * Name:	gsd-wwan
 * State:	S (sleeping)
 * Pid:	14800
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  396200 kB
 * VmRSS:	    6308 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-wwan 
 *
 * === Process 14829 ===
 * Name:	geoclue
 * State:	S (sleeping)
 * Pid:	14829
 * PPid:	1
 * Uid:	964	964	964	964
 * Gid:	964	964	964	964
 * VmSize:	  568200 kB
 * VmRSS:	   19280 kB
 * Threads:	4
 * Cmdline: /usr/lib/geoclue 
 *
 * === Process 14832 ===
 * Name:	goa-daemon
 * State:	S (sleeping)
 * Pid:	14832
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  571120 kB
 * VmRSS:	    9324 kB
 * Threads:	4
 * Cmdline: /usr/lib/goa-daemon 
 *
 * === Process 14852 ===
 * Name:	ibus-dconf
 * State:	S (sleeping)
 * Pid:	14852
 * PPid:	14743
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  243472 kB
 * VmRSS:	    4776 kB
 * Threads:	5
 * Cmdline: /usr/lib/ibus/ibus-dconf 
 *
 * === Process 14859 ===
 * Name:	ibus-extension-
 * State:	S (sleeping)
 * Pid:	14859
 * PPid:	14743
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  554408 kB
 * VmRSS:	   14496 kB
 * Threads:	6
 * Cmdline: /usr/lib/ibus/ibus-extension-gtk3 
 *
 * === Process 14877 ===
 * Name:	ibus-portal
 * State:	S (sleeping)
 * Pid:	14877
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  308888 kB
 * VmRSS:	    5180 kB
 * Threads:	4
 * Cmdline: /usr/lib/ibus/ibus-portal 
 *
 * === Process 14884 ===
 * Name:	evolution-alarm
 * State:	S (sleeping)
 * Pid:	14884
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  981600 kB
 * VmRSS:	   12388 kB
 * Threads:	8
 * Cmdline: /usr/lib/evolution-data-server/evolution-alarm-notify 
 *
 * === Process 14888 ===
 * Name:	localsearch-3
 * State:	S (sleeping)
 * Pid:	14888
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  618468 kB
 * VmRSS:	   20572 kB
 * Threads:	8
 * Cmdline: /usr/lib/localsearch-3 
 *
 * === Process 14898 ===
 * Name:	xdg-desktop-por
 * State:	S (sleeping)
 * Pid:	14898
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  560044 kB
 * VmRSS:	    8780 kB
 * Threads:	5
 * Cmdline: /usr/lib/xdg-desktop-portal 
 *
 * === Process 14910 ===
 * Name:	gjs
 * State:	S (sleeping)
 * Pid:	14910
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 2672364 kB
 * VmRSS:	   12728 kB
 * Threads:	12
 * Cmdline: /usr/bin/gjs -m /usr/share/gnome-shell/org.gnome.ScreenSaver 
 *
 * === Process 14911 ===
 * Name:	pipewire
 * State:	S (sleeping)
 * Pid:	14911
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   52192 kB
 * VmRSS:	    8632 kB
 * Threads:	2
 * Cmdline: /usr/bin/pipewire 
 *
 * === Process 14915 ===
 * Name:	wireplumber
 * State:	S (sleeping)
 * Pid:	14915
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  773452 kB
 * VmRSS:	   12724 kB
 * Threads:	7
 * Cmdline: /usr/bin/wireplumber 
 *
 * === Process 14930 ===
 * Name:	pipewire-pulse
 * State:	S (sleeping)
 * Pid:	14930
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  123624 kB
 * VmRSS:	   10008 kB
 * Threads:	2
 * Cmdline: /usr/bin/pipewire-pulse 
 *
 * === Process 14948 ===
 * Name:	evolution-calen
 * State:	S (sleeping)
 * Pid:	14948
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  820408 kB
 * VmRSS:	   31124 kB
 * Threads:	28
 * Cmdline: /usr/lib/evolution-calendar-factory 
 *
 * === Process 14979 ===
 * Name:	goa-identity-se
 * State:	S (sleeping)
 * Pid:	14979
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  394332 kB
 * VmRSS:	    5992 kB
 * Threads:	4
 * Cmdline: /usr/lib/goa-identity-service 
 *
 * === Process 14991 ===
 * Name:	xdg-document-po
 * State:	S (sleeping)
 * Pid:	14991
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  563544 kB
 * VmRSS:	    5460 kB
 * Threads:	7
 * Cmdline: /usr/lib/xdg-document-portal 
 *
 * === Process 15016 ===
 * Name:	fusermount3
 * State:	S (sleeping)
 * Pid:	15016
 * PPid:	14991
 * Uid:	1000	0	0	0
 * Gid:	1000	1000	1000	1000
 * VmSize:	    3016 kB
 * VmRSS:	    1596 kB
 * Threads:	1
 * Cmdline: fusermount3 -o rw,nosuid,nodev,fsname=portal,auto_unmount,subtype=portal -- /run/user/1000/doc 
 *
 * === Process 15035 ===
 * Name:	xdg-desktop-por
 * State:	S (sleeping)
 * Pid:	15035
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  666776 kB
 * VmRSS:	   13156 kB
 * Threads:	6
 * Cmdline: /usr/lib/xdg-desktop-portal-gnome 
 *
 * === Process 15048 ===
 * Name:	gvfs-udisks2-vo
 * State:	S (sleeping)
 * Pid:	15048
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  584980 kB
 * VmRSS:	    7296 kB
 * Threads:	5
 * Cmdline: /usr/lib/gvfs-udisks2-volume-monitor 
 *
 * === Process 15114 ===
 * Name:	evolution-addre
 * State:	S (sleeping)
 * Pid:	15114
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  624960 kB
 * VmRSS:	    6236 kB
 * Threads:	7
 * Cmdline: /usr/lib/evolution-addressbook-factory 
 *
 * === Process 15137 ===
 * Name:	gvfs-mtp-volume
 * State:	S (sleeping)
 * Pid:	15137
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  308476 kB
 * VmRSS:	    5352 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfs-mtp-volume-monitor 
 *
 * === Process 15151 ===
 * Name:	gvfs-gphoto2-vo
 * State:	S (sleeping)
 * Pid:	15151
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  310636 kB
 * VmRSS:	    5508 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfs-gphoto2-volume-monitor 
 *
 * === Process 15168 ===
 * Name:	gvfs-goa-volume
 * State:	S (sleeping)
 * Pid:	15168
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  308252 kB
 * VmRSS:	    5116 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfs-goa-volume-monitor 
 *
 * === Process 15176 ===
 * Name:	gvfs-afc-volume
 * State:	S (sleeping)
 * Pid:	15176
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  392112 kB
 * VmRSS:	    5200 kB
 * Threads:	5
 * Cmdline: /usr/lib/gvfs-afc-volume-monitor 
 *
 * === Process 15191 ===
 * Name:	ibus-engine-sim
 * State:	S (sleeping)
 * Pid:	15191
 * PPid:	14743
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  169672 kB
 * VmRSS:	    5508 kB
 * Threads:	4
 * Cmdline: /usr/lib/ibus/ibus-engine-simple 
 *
 * === Process 15219 ===
 * Name:	dconf-service
 * State:	S (sleeping)
 * Pid:	15219
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  167036 kB
 * VmRSS:	    3992 kB
 * Threads:	4
 * Cmdline: /usr/lib/dconf-service 
 *
 * === Process 15393 ===
 * Name:	gsd-printer
 * State:	S (sleeping)
 * Pid:	15393
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  474524 kB
 * VmRSS:	    6128 kB
 * Threads:	4
 * Cmdline: /usr/lib/gsd-printer 
 *
 * === Process 15915 ===
 * Name:	xdg-desktop-por
 * State:	S (sleeping)
 * Pid:	15915
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  545256 kB
 * VmRSS:	    9108 kB
 * Threads:	6
 * Cmdline: /usr/lib/xdg-desktop-portal-gtk 
 *
 * === Process 15939 ===
 * Name:	gvfsd-trash
 * State:	S (sleeping)
 * Pid:	15939
 * PPid:	14635
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  560960 kB
 * VmRSS:	    8168 kB
 * Threads:	5
 * Cmdline: /usr/lib/gvfsd-trash --spawner :1.10 /org/gtk/gvfs/exec_spaw/0 
 *
 * === Process 16863 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16863
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55889316 kB
 * VmRSS:	  273368 kB
 * Threads:	38
 * Cmdline: /opt/brave-bin/brave 
 *
 * === Process 16865 ===
 * Name:	chrome_crashpad
 * State:	S (sleeping)
 * Pid:	16865
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54550660 kB
 * VmRSS:	    1660 kB
 * Threads:	3
 * Cmdline: /opt/brave-bin/chrome_crashpad_handler --monitor-self --monitor-self-annotation=ptype=crashpad-handler --database=/home/kss/.config/BraveSoftware/Brave-Browser/Crash Reports --metrics-dir=/home/kss/.config/BraveSoftware/Brave-Browser --url=https://cr.brave.com --annotation=lsb-release=Manjaro Linux --annotation=plat=Linux --annotation=prod=Chrome_Linux --annotation=ver=153.1.95.102 --initial-client-fd=5 --shared-client-connection 
 *
 * === Process 16867 ===
 * Name:	chrome_crashpad
 * State:	S (sleeping)
 * Pid:	16867
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54541424 kB
 * VmRSS:	    1676 kB
 * Threads:	2
 * Cmdline: /opt/brave-bin/chrome_crashpad_handler --no-periodic-tasks --monitor-self-annotation=ptype=crashpad-handler --database=/home/kss/.config/BraveSoftware/Brave-Browser/Crash Reports --url=https://cr.brave.com --annotation=lsb-release=Manjaro Linux --annotation=plat=Linux --annotation=prod=Chrome_Linux --annotation=ver=153.1.95.102 --initial-client-fd=4 --shared-client-connection 
 *
 * === Process 16877 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16877
 * PPid:	16863
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55028840 kB
 * VmRSS:	    9048 kB
 * Threads:	1
 * Cmdline: /opt/brave-bin/brave --type=zygote --no-zygote-sandbox --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable 
 *
 * === Process 16878 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16878
 * PPid:	16863
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55028828 kB
 * VmRSS:	   10024 kB
 * Threads:	1
 * Cmdline: /opt/brave-bin/brave --type=zygote --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable 
 *
 * === Process 16880 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16880
 * PPid:	16878
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55028852 kB
 * VmRSS:	    3292 kB
 * Threads:	1
 * Cmdline: /opt/brave-bin/brave --type=zygote --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable 
 *
 * === Process 16910 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16910
 * PPid:	16877
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55583424 kB
 * VmRSS:	   95816 kB
 * Threads:	28
 * Cmdline: /opt/brave-bin/brave --type=gpu-process --ozone-platform=wayland --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable --gpu-preferences=YAAAAAAAAAAgAQAEAAAAAAAAAAAAAGAASAAAAAAAAAABAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAAwAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAMAAAAAQAAAAAAAAAAAAAACAAAAAAAAAAIAAAAAAAAAA== --shared-files --metrics-shmem-handle=4,i,16835258528908107854,15939954936640175423,262144 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190708988185955192 
 *
 * === Process 16912 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16912
 * PPid:	16863
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55100028 kB
 * VmRSS:	   65228 kB
 * Threads:	14
 * Cmdline: /opt/brave-bin/brave --type=utility --utility-sub-type=network.mojom.NetworkService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,6999236210282464152,9300890352517682816,524288 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190708989122997041 
 *
 * === Process 16949 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16949
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55112692 kB
 * VmRSS:	   23092 kB
 * Threads:	11
 * Cmdline: /opt/brave-bin/brave --type=utility --utility-sub-type=storage.mojom.StorageService --lang=en-US --service-sandbox-type=utility --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,14283969285238492918,5214058841325088778,524288 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190708990060038890 
 *
 * === Process 16967 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16967
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1520465416 kB
 * VmRSS:	   39588 kB
 * Threads:	16
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --extension-process --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=5 --launch-time-ticks=4578533271 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,14187576408617777496,18349590634658880218,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190708990997080739 
 *
 * === Process 16968 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	16968
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1520494532 kB
 * VmRSS:	  120584 kB
 * Threads:	16
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --extension-process --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=6 --launch-time-ticks=4578537180 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,14067733508949421220,5859985454940506496,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190708991934122588 
 *
 * === Process 17262 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	17262
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518448300 kB
 * VmRSS:	   24564 kB
 * Threads:	15
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --extension-process --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=22 --launch-time-ticks=4579909566 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,2099996704602481878,15444451946499329346,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709006926792172 
 *
 * === Process 18121 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	18121
 * PPid:	16863
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55127628 kB
 * VmRSS:	   21296 kB
 * Threads:	10
 * Cmdline: /opt/brave-bin/brave --type=utility --utility-sub-type=audio.mojom.AudioService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,10338979910202646250,11720154022536035321,524288 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709037849173189 
 *
 * === Process 19827 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19827
 * PPid:	14622
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518882752 kB
 * VmRSS:	   30736 kB
 * Threads:	45
 * Cmdline: /usr/lib/electron43/electron --ozone-platform-hint=auto /usr/lib/obsidian/app.asar 
 *
 * === Process 19836 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19836
 * PPid:	19827
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54896396 kB
 * VmRSS:	    6320 kB
 * Threads:	1
 * Cmdline: /usr/lib/electron43/electron --type=zygote --no-zygote-sandbox 
 *
 * === Process 19837 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19837
 * PPid:	19827
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54896388 kB
 * VmRSS:	    6240 kB
 * Threads:	1
 * Cmdline: /usr/lib/electron43/electron --type=zygote 
 *
 * === Process 19839 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19839
 * PPid:	19837
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54896388 kB
 * VmRSS:	     628 kB
 * Threads:	1
 * Cmdline: /usr/lib/electron43/electron --type=zygote 
 *
 * === Process 19875 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19875
 * PPid:	19836
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	55340012 kB
 * VmRSS:	   29676 kB
 * Threads:	23
 * Cmdline: /usr/lib/electron43/electron --type=gpu-process --ozone-platform=wayland --render-node-override=/dev/dri/renderD128 --enable-crash-reporter=aad7893b-2306-40bd-ac53-da68c167f5d5,no_channel --user-data-dir=/home/kss/.config/obsidian --gpu-preferences=WAAAAAAAAAAgAQAEAAAAAAAAAAAAAGAAQAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAACAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAACAAAAAAAAAAIAAAAAAAAAA== --shared-files --field-trial-handle=3,i,12408063106707769023,12403791020598801975,262144 --enable-features=GlobalShortcutsPortalPreferredTrigger,PdfUseShowSaveFilePicker,SharedArrayBuffer --disable-features=DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,11287980083928976667,8473468722234890681,4 --trace-process-track-uuid=3190708988185955192 --log-level=3 
 *
 * === Process 19879 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19879
 * PPid:	19827
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	54970764 kB
 * VmRSS:	   18708 kB
 * Threads:	10
 * Cmdline: /usr/lib/electron43/electron --type=utility --utility-sub-type=network.mojom.NetworkService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --enable-crash-reporter=aad7893b-2306-40bd-ac53-da68c167f5d5,no_channel --user-data-dir=/home/kss/.config/obsidian --standard-schemes=app --secure-schemes=app --cors-schemes=app --fetch-schemes=app --streaming-schemes=app --code-cache-schemes=app --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,12408063106707769023,12403791020598801975,262144 --enable-features=GlobalShortcutsPortalPreferredTrigger,PdfUseShowSaveFilePicker,SharedArrayBuffer --disable-features=DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,11287980083928976667,8473468722234890681,4 --trace-process-track-uuid=3190708989122997041 --log-level=3 
 *
 * === Process 19884 ===
 * Name:	electron
 * State:	S (sleeping)
 * Pid:	19884
 * PPid:	19827
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1522441808 kB
 * VmRSS:	   36576 kB
 * Threads:	22
 * Cmdline: /usr/lib/electron43/electron --type=renderer --enable-crash-reporter=aad7893b-2306-40bd-ac53-da68c167f5d5,no_channel --user-data-dir=/home/kss/.config/obsidian --standard-schemes=app --secure-schemes=app --cors-schemes=app --fetch-schemes=app --streaming-schemes=app --code-cache-schemes=app --app-path=/usr/lib/obsidian/app.asar --no-sandbox --no-zygote --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=4 --time-ticks-at-unix-epoch=-1789984277963201 --launch-time-ticks=6146301451 --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,12408063106707769023,12403791020598801975,262144 --enable-features=GlobalShortcutsPortalPreferredTrigger,PdfUseShowSaveFilePicker,SharedArrayBuffer --disable-features=DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,11287980083928976667,8473468722234890681,4 --trace-process-track-uuid=3190708990060038890 --log-level=3 
 *
 * === Process 20849 ===
 * Name:	gvfsd-network
 * State:	S (sleeping)
 * Pid:	20849
 * PPid:	14635
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  405908 kB
 * VmRSS:	    5816 kB
 * Threads:	5
 * Cmdline: /usr/lib/gvfsd-network --spawner :1.10 /org/gtk/gvfs/exec_spaw/1 
 *
 * === Process 20858 ===
 * Name:	gvfsd-dnssd
 * State:	S (sleeping)
 * Pid:	20858
 * PPid:	14635
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  405452 kB
 * VmRSS:	    6200 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfsd-dnssd --spawner :1.10 /org/gtk/gvfs/exec_spaw/2 
 *
 * === Process 20869 ===
 * Name:	python3
 * State:	S (sleeping)
 * Pid:	20869
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   47136 kB
 * VmRSS:	   10340 kB
 * Threads:	1
 * Cmdline: python3 /usr/bin/wsdd --no-host --discovery --listen /run/user/1000/gvfsd/wsdd 
 *
 * === Process 20905 ===
 * Name:	Xwayland
 * State:	S (sleeping)
 * Pid:	20905
 * PPid:	14622
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  912500 kB
 * VmRSS:	   24772 kB
 * Threads:	13
 * Cmdline: /usr/bin/Xwayland :0 -rootless -noreset -accessx -core -auth /run/user/1000/.mutter-Xwaylandauth.UYM8V3 -listenfd 4 -listenfd 5 -displayfd 6 -initfd 7 -byteswappedclients -enable-ei-portal 
 *
 * === Process 20916 ===
 * Name:	gsd-xsettings
 * State:	S (sleeping)
 * Pid:	20916
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  255156 kB
 * VmRSS:	    5936 kB
 * Threads:	5
 * Cmdline: /usr/lib/gsd-xsettings 
 *
 * === Process 20924 ===
 * Name:	mutter-x11-fram
 * State:	S (sleeping)
 * Pid:	20924
 * PPid:	14622
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  937236 kB
 * VmRSS:	   30412 kB
 * Threads:	13
 * Cmdline: /usr/lib/mutter-x11-frames 
 *
 * === Process 20943 ===
 * Name:	ibus-x11
 * State:	S (sleeping)
 * Pid:	20943
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  184016 kB
 * VmRSS:	    3156 kB
 * Threads:	4
 * Cmdline: /usr/lib/ibus/ibus-x11 
 *
 * === Process 27378 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	27378
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518456840 kB
 * VmRSS:	  100636 kB
 * Threads:	15
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=229 --launch-time-ticks=16494838559 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,17077243117586030551,11377822633129749638,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709200894454915 
 *
 * === Process 29790 ===
 * Name:	kworker/6:2-events
 * State:	I (idle)
 * Pid:	29790
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 30745 ===
 * Name:	gvfsd-http
 * State:	S (sleeping)
 * Pid:	30745
 * PPid:	14635
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  335296 kB
 * VmRSS:	    6640 kB
 * Threads:	4
 * Cmdline: /usr/lib/gvfsd-http --spawner :1.10 /org/gtk/gvfs/exec_spaw/5 
 *
 * === Process 30801 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	30801
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518492844 kB
 * VmRSS:	   53792 kB
 * Threads:	15
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=291 --launch-time-ticks=23313651092 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,10410487303493016568,14997317146952900609,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709258991049553 
 *
 * === Process 31182 ===
 * Name:	gcr-ssh-agent
 * State:	S (sleeping)
 * Pid:	31182
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  254056 kB
 * VmRSS:	    4348 kB
 * Threads:	3
 * Cmdline: /usr/lib/gcr-ssh-agent --base-dir /run/user/1000/gcr 
 *
 * === Process 31186 ===
 * Name:	ssh-agent
 * State:	S (sleeping)
 * Pid:	31186
 * PPid:	31182
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   12728 kB
 * VmRSS:	    1800 kB
 * Threads:	1
 * Cmdline: /usr/bin/ssh-agent -D -a /run/user/1000/gcr/.ssh 
 *
 * === Process 33220 ===
 * Name:	bwrap
 * State:	S (sleeping)
 * Pid:	33220
 * PPid:	14622
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    3284 kB
 * VmRSS:	    1592 kB
 * Threads:	1
 * Cmdline: bwrap --unshare-all --die-with-parent --chdir / --ro-bind /usr /usr --dev /dev --ro-bind-try /etc/ld.so.cache /etc/ld.so.cache --ro-bind-try /nix/store /nix/store --tmpfs /tmp-home --tmpfs /tmp-run --clearenv --setenv HOME /tmp-home --setenv XDG_RUNTIME_DIR /tmp-run --setenv XDG_RUNTIME_DIR /run/user/1000 --symlink /usr/lib /lib --symlink /usr/lib /lib64 --seccomp 155 /usr/lib/glycin-loaders/2+/glycin-image-rs --dbus-fd 154 
 *
 * === Process 33222 ===
 * Name:	bwrap
 * State:	S (sleeping)
 * Pid:	33222
 * PPid:	33220
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    3284 kB
 * VmRSS:	     944 kB
 * Threads:	1
 * Cmdline: bwrap --unshare-all --die-with-parent --chdir / --ro-bind /usr /usr --dev /dev --ro-bind-try /etc/ld.so.cache /etc/ld.so.cache --ro-bind-try /nix/store /nix/store --tmpfs /tmp-home --tmpfs /tmp-run --clearenv --setenv HOME /tmp-home --setenv XDG_RUNTIME_DIR /tmp-run --setenv XDG_RUNTIME_DIR /run/user/1000 --symlink /usr/lib /lib --symlink /usr/lib /lib64 --seccomp 155 /usr/lib/glycin-loaders/2+/glycin-image-rs --dbus-fd 154 
 *
 * === Process 33223 ===
 * Name:	glycin-image-rs
 * State:	S (sleeping)
 * Pid:	33223
 * PPid:	33222
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  145628 kB
 * VmRSS:	    1688 kB
 * Threads:	3
 * Cmdline: /usr/lib/glycin-loaders/2+/glycin-image-rs --dbus-fd 154 
 *
 * === Process 37143 ===
 * Name:	kworker/u32:0-sdma0
 * State:	I (idle)
 * Pid:	37143
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 38926 ===
 * Name:	kworker/1:0-events
 * State:	I (idle)
 * Pid:	38926
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 39398 ===
 * Name:	kworker/u33:15-btrfs-endio
 * State:	I (idle)
 * Pid:	39398
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 39400 ===
 * Name:	kworker/u33:19-kcryptd-253:1-2
 * State:	I (idle)
 * Pid:	39400
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42548 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	42548
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518465440 kB
 * VmRSS:	   75012 kB
 * Threads:	16
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --extension-process --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=460 --launch-time-ticks=29180892089 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,17890007358502334990,4085289265271358462,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709417351122034 
 *
 * === Process 42616 ===
 * Name:	kworker/2:2-inode_switch_wbs
 * State:	I (idle)
 * Pid:	42616
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42654 ===
 * Name:	kworker/u34:3-btrfs-endio
 * State:	I (idle)
 * Pid:	42654
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42788 ===
 * Name:	kworker/u36:10-ttm
 * State:	I (idle)
 * Pid:	42788
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42852 ===
 * Name:	kworker/u37:9-ttm
 * State:	I (idle)
 * Pid:	42852
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42940 ===
 * Name:	kworker/4:2-mm_percpu_wq
 * State:	I (idle)
 * Pid:	42940
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 42999 ===
 * Name:	kworker/u34:7-btrfs-endio
 * State:	I (idle)
 * Pid:	42999
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43003 ===
 * Name:	kworker/u33:9-btrfs-endio-write
 * State:	I (idle)
 * Pid:	43003
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43006 ===
 * Name:	kworker/u33:17-btrfs-endio
 * State:	I (idle)
 * Pid:	43006
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43009 ===
 * Name:	kworker/u33:22-kcryptd-253:1-2
 * State:	I (idle)
 * Pid:	43009
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43015 ===
 * Name:	kworker/u33:28-btrfs-endio
 * State:	I (idle)
 * Pid:	43015
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43018 ===
 * Name:	kworker/u33:31-btrfs-endio-write
 * State:	I (idle)
 * Pid:	43018
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43142 ===
 * Name:	kworker/3:0-events
 * State:	I (idle)
 * Pid:	43142
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43176 ===
 * Name:	kworker/u37:0-ttm
 * State:	I (idle)
 * Pid:	43176
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43195 ===
 * Name:	kworker/5:2-mm_percpu_wq
 * State:	I (idle)
 * Pid:	43195
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43250 ===
 * Name:	kworker/u34:8-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	43250
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43252 ===
 * Name:	kworker/u34:10-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	43252
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43254 ===
 * Name:	kworker/u34:12-kcryptd-253:1-2
 * State:	I (idle)
 * Pid:	43254
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43258 ===
 * Name:	kworker/u34:17-kcryptd-253:0-1
 * State:	I (idle)
 * Pid:	43258
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43260 ===
 * Name:	kworker/u34:19-btrfs-endio-write
 * State:	I (idle)
 * Pid:	43260
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43268 ===
 * Name:	kworker/u34:30-btrfs-endio
 * State:	I (idle)
 * Pid:	43268
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43269 ===
 * Name:	kworker/u34:31-btrfs-endio
 * State:	I (idle)
 * Pid:	43269
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43270 ===
 * Name:	kworker/u34:32-kcryptd-253:0-1
 * State:	I (idle)
 * Pid:	43270
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43577 ===
 * Name:	gjs
 * State:	S (sleeping)
 * Pid:	43577
 * PPid:	14622
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	 3059880 kB
 * VmRSS:	   34696 kB
 * Threads:	21
 * Cmdline: gjs /usr/share/gnome-shell/extensions/ding@rastersoft.com/app/ding.js -E -P /usr/share/gnome-shell/extensions/ding@rastersoft.com/app 
 *
 * === Process 43675 ===
 * Name:	kworker/u36:4-ttm
 * State:	I (idle)
 * Pid:	43675
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 43764 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	43764
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518439764 kB
 * VmRSS:	   83052 kB
 * Threads:	14
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=470 --launch-time-ticks=31661077379 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,16355042570900142494,4170651214642952132,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709426721540524 
 *
 * === Process 43800 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	43800
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1520462972 kB
 * VmRSS:	  196644 kB
 * Threads:	16
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=474 --launch-time-ticks=31664797652 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,3612903882454377209,144465617967827757,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709430469707920 
 *
 * === Process 43887 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43887
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518925556 kB
 * VmRSS:	  104704 kB
 * Threads:	56
 * Cmdline: /usr/share/code/code 
 *
 * === Process 43890 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43890
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	50663828 kB
 * VmRSS:	   16052 kB
 * Threads:	1
 * Cmdline: /usr/share/code/code --type=zygote --no-zygote-sandbox 
 *
 * === Process 43891 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43891
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	50663816 kB
 * VmRSS:	   16296 kB
 * Threads:	1
 * Cmdline: /usr/share/code/code --type=zygote 
 *
 * === Process 43893 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43893
 * PPid:	43891
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	50663840 kB
 * VmRSS:	    2004 kB
 * Threads:	1
 * Cmdline: /usr/share/code/code --type=zygote 
 *
 * === Process 43911 ===
 * Name:	chrome_crashpad
 * State:	S (sleeping)
 * Pid:	43911
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	50356848 kB
 * VmRSS:	    2976 kB
 * Threads:	3
 * Cmdline: /usr/share/code/chrome_crashpad_handler --monitor-self-annotation=ptype=crashpad-handler --no-rate-limit --database=/home/kss/.config/Code/Crashpad --url=appcenter://code?aid=fba07a4d-84bd-4fc8-a125-9640fc8ce171&uid=711d375d-2e95-464a-a7e1-dbd53517c678&iid=711d375d-2e95-464a-a7e1-dbd53517c678&sid=711d375d-2e95-464a-a7e1-dbd53517c678 --annotation=_companyName=Microsoft --annotation=_productName=VSCode --annotation=_version=1.138.0 --annotation=lsb-release=Manjaro Linux --annotation=plat=Linux --annotation=prod=Electron --annotation=ver=42.10.0 --initial-client-fd=43 --shared-client-connection 
 *
 * === Process 43933 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43933
 * PPid:	43890
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	51133964 kB
 * VmRSS:	   72024 kB
 * Threads:	21
 * Cmdline: /usr/share/code/code --type=gpu-process --ozone-platform=wayland --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --gpu-preferences=UAAAAAAAAAAgAQAEAAAAAAAAAAAAAGAAAQAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAYAAAAAAAAABgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAgAAAAAAAAA --shared-files --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708988185955192 
 *
 * === Process 43936 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	43936
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	50755240 kB
 * VmRSS:	   39692 kB
 * Threads:	12
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=network.mojom.NetworkService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708989122997041 
 *
 * === Process 44006 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	44006
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518432716 kB
 * VmRSS:	   39648 kB
 * Threads:	19
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=node.mojom.NodeService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708990997080739 
 *
 * === Process 44026 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	44026
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518418644 kB
 * VmRSS:	   45624 kB
 * Threads:	19
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=node.mojom.NodeService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708991934122588 
 *
 * === Process 44088 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	44088
 * PPid:	44006
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518495332 kB
 * VmRSS:	   23416 kB
 * Threads:	29
 * Cmdline: /usr/share/code/code /usr/share/code/resources/app/node_modules.asar.unpacked/@github/copilot-linux-x64/index.js --headless --no-auto-update --log-level info --stdio --no-auto-login 
 *
 * === Process 44190 ===
 * Name:	dconf
 * State:	S (sleeping)
 * Pid:	44190
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  242280 kB
 * VmRSS:	    7120 kB
 * Threads:	5
 * Cmdline: dconf watch /system/proxy/ 
 *
 * === Process 44294 ===
 * Name:	kworker/u33:2-btrfs-endio-write
 * State:	I (idle)
 * Pid:	44294
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 44297 ===
 * Name:	kworker/u33:5-btrfs-endio
 * State:	I (idle)
 * Pid:	44297
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 44974 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	44974
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518444108 kB
 * VmRSS:	   47020 kB
 * Threads:	20
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=node.mojom.NodeService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708996619331833 
 *
 * === Process 45200 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	45200
 * PPid:	43893
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1522408780 kB
 * VmRSS:	  310600 kB
 * Threads:	17
 * Cmdline: /usr/share/code/code --type=renderer --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --app-path=/usr/share/code/resources/app --enable-sandbox --enable-blink-features=HighlightAPI --max-active-webgl-contexts=32 --ozone-platform=wayland --disable-blink-features=FontMatchingCTMigration,StandardizedBrowserZoom, --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=12 --time-ticks-at-unix-epoch=-1790058115450372 --launch-time-ticks=31917633232 --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708997556373682 --vscode-window-config=vscode:8db1cfc8-f7bd-4d48-9165-e8a133b9934f 
 *
 * === Process 45241 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	45241
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518435660 kB
 * VmRSS:	   36064 kB
 * Threads:	21
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=node.mojom.NodeService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708998493415531 
 *
 * === Process 45260 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	45260
 * PPid:	43887
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518885472 kB
 * VmRSS:	  366732 kB
 * Threads:	27
 * Cmdline: /usr/share/code/code --type=utility --utility-sub-type=node.mojom.NodeService --lang=en-US --service-sandbox-type=none --render-node-override=/dev/dri/renderD128 --dns-result-order=ipv4first --experimental-network-inspection --inspect-port=0 --crashpad-handler-pid=43911 --enable-crash-reporter=80825b5d-a146-496d-84a2-0be716bf950b,no_channel --user-data-dir=/home/kss/.config/Code --standard-schemes=vscode-webview,vscode-file --enable-sandbox --secure-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --cors-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --fetch-schemes=vscode-webview,vscode-file,vscode-remote-resource,vscode-managed-remote-resource --service-worker-schemes=vscode-webview --code-cache-schemes=vscode-webview,vscode-file --shared-files=v8_context_snapshot_data:100 --field-trial-handle=3,i,4279705307276234466,1695740727825373178,262144 --enable-features=DocumentPolicyIncludeJSCallStacksInCrashReports,EarlyEstablishGpuChannel,EstablishGpuChannelAsync,GlobalShortcutsPortal,PdfUseShowSaveFilePicker --disable-features=CalculateNativeWinOcclusion,DropInputEventsWhilePaintHolding,LocalNetworkAccessChecks,ScreenAIOCREnabled,SpareRendererForSitePerProcess,TraceSiteInstanceGetProcessCreation --variations-seed-version --pseudonymization-salt-handle=7,i,12383846308366440919,6567301429175511407,4 --trace-process-track-uuid=3190708999430457380 
 *
 * === Process 45449 ===
 * Name:	codex
 * State:	S (sleeping)
 * Pid:	45449
 * PPid:	45260
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  822304 kB
 * VmRSS:	   35280 kB
 * Threads:	23
 * Cmdline: /home/kss/.vscode/extensions/openai.chatgpt-26.908.40401-linux-x64/bin/linux-x86_64/codex -c features.code_mode_host=true app-server --analytics-default-enabled 
 *
 * === Process 46155 ===
 * Name:	cpptools
 * State:	S (sleeping)
 * Pid:	46155
 * PPid:	45260
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	  250904 kB
 * VmRSS:	   87952 kB
 * Threads:	38
 * Cmdline: /home/kss/.vscode/extensions/ms-vscode.cpptools-1.34.4-linux-x64/bin/cpptools 
 *
 * === Process 46186 ===
 * Name:	kworker/1:2-cgroup_free
 * State:	I (idle)
 * Pid:	46186
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46219 ===
 * Name:	cpptools-srv2
 * State:	S (sleeping)
 * Pid:	46219
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   82128 kB
 * VmRSS:	   14296 kB
 * Threads:	10
 * Cmdline: /home/kss/.vscode/extensions/ms-vscode.cpptools-1.34.4-linux-x64/bin/cpptools-srv2 -s {C8EA6122-762B-4A0E-9E57-82D072ED02B6} -p 46155 -i TagParser -t /tmp/.vscode-cpptools-1000/cpptools/crashes/97851425cb67bf4c7d0501c3f68d5437 
 *
 * === Process 46239 ===
 * Name:	cpptools-srv2
 * State:	S (sleeping)
 * Pid:	46239
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   81468 kB
 * VmRSS:	   12432 kB
 * Threads:	10
 * Cmdline: /home/kss/.vscode/extensions/ms-vscode.cpptools-1.34.4-linux-x64/bin/cpptools-srv2 -s {D75F6F72-CB3B-4A2C-AA1E-41315DADA92F} -p 46155 -i TagParser -t /tmp/.vscode-cpptools-1000/cpptools/crashes/97851425cb67bf4c7d0501c3f68d5437 
 *
 * === Process 46443 ===
 * Name:	kworker/u37:1-ttm
 * State:	I (idle)
 * Pid:	46443
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46465 ===
 * Name:	kworker/u36:0-ttm
 * State:	I (idle)
 * Pid:	46465
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46522 ===
 * Name:	kworker/u37:2-ttm
 * State:	I (idle)
 * Pid:	46522
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46524 ===
 * Name:	kworker/6:0-events
 * State:	I (idle)
 * Pid:	46524
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46546 ===
 * Name:	kworker/u33:3-btrfs-endio
 * State:	I (idle)
 * Pid:	46546
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46548 ===
 * Name:	kworker/u37:3-ttm
 * State:	I (idle)
 * Pid:	46548
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46555 ===
 * Name:	kworker/7:0-mm_percpu_wq
 * State:	I (idle)
 * Pid:	46555
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46557 ===
 * Name:	kworker/u36:1-ttm
 * State:	I (idle)
 * Pid:	46557
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46576 ===
 * Name:	kworker/u37:4-ttm
 * State:	I (idle)
 * Pid:	46576
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46577 ===
 * Name:	kworker/u37:5-ttm
 * State:	I (idle)
 * Pid:	46577
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46608 ===
 * Name:	kworker/2:0-events
 * State:	I (idle)
 * Pid:	46608
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46650 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	46650
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1520449360 kB
 * VmRSS:	  306520 kB
 * Threads:	16
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=481 --launch-time-ticks=32475762167 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,8747437985099539194,8307446659704375004,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709437029000863 
 *
 * === Process 46689 ===
 * Name:	kworker/u37:6-ttm
 * State:	I (idle)
 * Pid:	46689
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46692 ===
 * Name:	kworker/u37:7-ttm
 * State:	I (idle)
 * Pid:	46692
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46712 ===
 * Name:	kworker/u36:2-ttm
 * State:	I (idle)
 * Pid:	46712
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46867 ===
 * Name:	qemu-system-x86
 * State:	S (sleeping)
 * Pid:	46867
 * PPid:	1
 * Uid:	961	961	961	961
 * Gid:	961	961	961	961
 * VmSize:	 4781972 kB
 * VmRSS:	  987448 kB
 * Threads:	53
 * Cmdline: /usr/bin/qemu-system-x86_64 -name guest=win10,debug-threads=on -S -object {"qom-type":"secret","id":"masterKey0","format":"raw","file":"/var/lib/libvirt/qemu/domain-3-win10/master-key.aes"} -machine pc-q35-9.1,usb=off,vmport=off,dump-guest-core=off,memory-backend=pc.ram,hpet=off,acpi=on -accel kvm -cpu qemu64,hv-time=on,hv-relaxed=on,hv-vapic=on,hv-spinlocks=0x1fff -m size=3151872k -object {"qom-type":"memory-backend-memfd","id":"pc.ram","share":true,"x-use-canonical-path-for-ramblock-id":false,"size":3227516928} -overcommit mem-lock=off -smp 4,sockets=4,cores=1,threads=1 -uuid e5559068-f337-4112-b4ff-a0854ed34f74 -no-user-config -nodefaults -chardev socket,id=charmonitor,fd=32,server=on,wait=off -object {"qom-type":"monitor-qmp","id":"monitor","chardev":"charmonitor"} -rtc base=localtime,driftfix=slew -global kvm-pit.lost_tick_policy=delay -no-shutdown -global ICH9-LPC.disable_s3=1 -global ICH9-LPC.disable_s4=1 -boot strict=on -device {"driver":"pcie-root-port","port":16,"chassis":1,"id":"pci.1","bus":"pcie.0","multifunction":true,"addr":"0x2"} -device {"driver":"pcie-root-port","port":17,"chassis":2,"id":"pci.2","bus":"pcie.0","addr":"0x2.0x1"} -device {"driver":"pcie-root-port","port":18,"chassis":3,"id":"pci.3","bus":"pcie.0","addr":"0x2.0x2"} -device {"driver":"pcie-root-port","port":19,"chassis":4,"id":"pci.4","bus":"pcie.0","addr":"0x2.0x3"} -device {"driver":"pcie-root-port","port":20,"chassis":5,"id":"pci.5","bus":"pcie.0","addr":"0x2.0x4"} -device {"driver":"pcie-root-port","port":21,"chassis":6,"id":"pci.6","bus":"pcie.0","addr":"0x2.0x5"} -device {"driver":"pcie-root-port","port":22,"chassis":7,"id":"pci.7","bus":"pcie.0","addr":"0x2.0x6"} -device {"driver":"pcie-root-port","port":23,"chassis":8,"id":"pci.8","bus":"pcie.0","addr":"0x2.0x7"} -device {"driver":"pcie-root-port","port":24,"chassis":9,"id":"pci.9","bus":"pcie.0","multifunction":true,"addr":"0x3"} -device {"driver":"pcie-root-port","port":25,"chassis":10,"id":"pci.10","bus":"pcie.0","addr":"0x3.0x1"} -device {"driver":"pcie-root-port","port":26,"chassis":11,"id":"pci.11","bus":"pcie.0","addr":"0x3.0x2"} -device {"driver":"pcie-root-port","port":27,"chassis":12,"id":"pci.12","bus":"pcie.0","addr":"0x3.0x3"} -device {"driver":"pcie-root-port","port":28,"chassis":13,"id":"pci.13","bus":"pcie.0","addr":"0x3.0x4"} -device {"driver":"pcie-root-port","port":29,"chassis":14,"id":"pci.14","bus":"pcie.0","addr":"0x3.0x5"} -device {"driver":"qemu-xhci","p2":15,"p3":15,"id":"usb","bus":"pci.2","addr":"0x0"} -device {"driver":"virtio-serial-pci","id":"virtio-serial0","bus":"pci.3","addr":"0x0"} -blockdev {"driver":"file","filename":"/home/kss/.libvirt-images/mal10.qcow2","node-name":"libvirt-1-storage","auto-read-only":true,"discard":"unmap"} -blockdev {"node-name":"libvirt-1-format","read-only":false,"driver":"qcow2","file":"libvirt-1-storage","backing":null} -device {"driver":"ide-hd","bus":"ide.0","drive":"libvirt-1-format","id":"sata0-0-0","bootindex":1} -netdev {"type":"tap","fd":"33","id":"hostnet0"} -device {"driver":"e1000e","netdev":"hostnet0","id":"net0","mac":"52:54:00:08:9f:48","bus":"pci.1","addr":"0x0"} -chardev pty,id=charserial0 -device {"driver":"isa-serial","chardev":"charserial0","id":"serial0","index":0} -chardev spicevmc,id=charchannel0,name=vdagent -device {"driver":"virtserialport","bus":"virtio-serial0.0","nr":1,"chardev":"charchannel0","id":"channel0","name":"com.redhat.spice.0"} -audiodev {"id":"audio1","driver":"spice"} -spice port=5900,addr=127.0.0.1,disable-ticketing=on,image-compression=off,seamless-migration=on -device {"driver":"qxl-vga","id":"video0","max_outputs":1,"ram_size":67108864,"vram_size":67108864,"vram64_size_mb":0,"vgamem_mb":16,"bus":"pcie.0","addr":"0x1"} -device {"driver":"ich9-intel-hda","id":"sound0","bus":"pcie.0","addr":"0x1b"} -device {"driver":"hda-duplex","id":"sound0-codec0","bus":"sound0.0","cad":0,"audiodev":"audio1"} -global ICH9-LPC.noreboot=off -watchdog-action reset -chardev spicevmc,id=charredir0,name=usbredir -device {"driver":"usb-redir","chardev":"charredir0","id":"redir0","bus":"usb.0","port":"2"} -device {"driver":"virtio-balloon-pci","id":"balloon0","bus":"pci.4","addr":"0x0"} -sandbox on,obsolete=deny,elevateprivileges=deny,spawn=deny,resourcecontrol=deny -msg timestamp=on 
 *
 * === Process 46889 ===
 * Name:	kvm-pit/46867
 * State:	S (sleeping)
 * Pid:	46889
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46934 ===
 * Name:	kworker/0:4-events
 * State:	I (idle)
 * Pid:	46934
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46936 ===
 * Name:	kworker/u34:0-btrfs-endio
 * State:	I (idle)
 * Pid:	46936
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 46967 ===
 * Name:	kworker/u33:6-btrfs-endio
 * State:	I (idle)
 * Pid:	46967
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47018 ===
 * Name:	kworker/u34:1-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47018
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47019 ===
 * Name:	kworker/u33:7-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47019
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47020 ===
 * Name:	kworker/u33:8-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47020
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47022 ===
 * Name:	kworker/5:0-events
 * State:	I (idle)
 * Pid:	47022
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47035 ===
 * Name:	kworker/u34:5-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47035
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47079 ===
 * Name:	kworker/u33:10-btrfs-endio-write
 * State:	I (idle)
 * Pid:	47079
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47082 ===
 * Name:	kworker/u36:5-ttm
 * State:	I (idle)
 * Pid:	47082
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47083 ===
 * Name:	kworker/u36:6-ttm
 * State:	I (idle)
 * Pid:	47083
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47086 ===
 * Name:	kworker/u36:9-ttm
 * State:	I (idle)
 * Pid:	47086
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47088 ===
 * Name:	kworker/u36:12-ttm
 * State:	I (idle)
 * Pid:	47088
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47089 ===
 * Name:	kworker/u36:13-rb_allocator
 * State:	I (idle)
 * Pid:	47089
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47092 ===
 * Name:	kworker/u37:8-ttm
 * State:	I (idle)
 * Pid:	47092
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47093 ===
 * Name:	kworker/u37:10-ttm
 * State:	I (idle)
 * Pid:	47093
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47094 ===
 * Name:	kworker/u36:14-ttm
 * State:	I (idle)
 * Pid:	47094
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47095 ===
 * Name:	kworker/u36:15-ttm
 * State:	I (idle)
 * Pid:	47095
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47098 ===
 * Name:	kworker/u32:2-comp_1.1.1
 * State:	I (idle)
 * Pid:	47098
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47108 ===
 * Name:	code
 * State:	S (sleeping)
 * Pid:	47108
 * PPid:	45260
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518325220 kB
 * VmRSS:	   44436 kB
 * Threads:	8
 * Cmdline: /usr/share/code/code /usr/share/code/resources/app/extensions/json-language-features/server/dist/node/jsonServerMain --node-ipc --clientProcessId=45260 
 *
 * === Process 47181 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	47181
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	71847968 kB
 * VmRSS:	   35688 kB
 * Threads:	6
 * Cmdline: /opt/brave-bin/brave --type=utility --utility-sub-type=data_decoder.mojom.DataDecoderService --lang=en-US --service-sandbox-type=service --render-node-override=/dev/dri/renderD128 --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --change-stack-guard-on-fork=enable --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,5156748174461405635,14553545441536622014,524288 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709441714210108 
 *
 * === Process 47203 ===
 * Name:	kworker/3:1-events
 * State:	I (idle)
 * Pid:	47203
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47231 ===
 * Name:	kworker/u37:11-ttm
 * State:	I (idle)
 * Pid:	47231
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47232 ===
 * Name:	kworker/u37:12-ttm
 * State:	I (idle)
 * Pid:	47232
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47233 ===
 * Name:	kworker/u37:13-ttm
 * State:	I (idle)
 * Pid:	47233
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47238 ===
 * Name:	kworker/4:0-events
 * State:	I (idle)
 * Pid:	47238
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47247 ===
 * Name:	bash
 * State:	S (sleeping)
 * Pid:	47247
 * PPid:	44974
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    9420 kB
 * VmRSS:	    6760 kB
 * Threads:	1
 * Cmdline: /usr/bin/bash --init-file /usr/share/code/resources/app/out/vs/workbench/contrib/terminal/common/scripts/shellIntegration-bash.sh 
 *
 * === Process 47291 ===
 * Name:	kworker/7:2
 * State:	I (idle)
 * Pid:	47291
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47302 ===
 * Name:	kworker/u37:14-ttm
 * State:	I (idle)
 * Pid:	47302
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47303 ===
 * Name:	kworker/u37:15-ttm
 * State:	I (idle)
 * Pid:	47303
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47697 ===
 * Name:	kworker/u32:1-comp_1.0.1
 * State:	I (idle)
 * Pid:	47697
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47698 ===
 * Name:	kworker/u34:4-btrfs-endio-write
 * State:	I (idle)
 * Pid:	47698
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47699 ===
 * Name:	kworker/u34:6-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47699
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47702 ===
 * Name:	kworker/u34:13-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	47702
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47703 ===
 * Name:	kworker/u34:14-btrfs-endio
 * State:	I (idle)
 * Pid:	47703
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47704 ===
 * Name:	kworker/0:0
 * State:	I (idle)
 * Pid:	47704
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47827 ===
 * Name:	kworker/6:1
 * State:	I (idle)
 * Pid:	47827
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47838 ===
 * Name:	kworker/1:1
 * State:	I (idle)
 * Pid:	47838
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47859 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	47859
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518495320 kB
 * VmRSS:	  370852 kB
 * Threads:	17
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=491 --launch-time-ticks=33142780355 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,13855407064161265180,319955324131065707,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709446399419353 
 *
 * === Process 47924 ===
 * Name:	kworker/5:1
 * State:	I (idle)
 * Pid:	47924
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47938 ===
 * Name:	kworker/u33:0-btrfs-endio-write
 * State:	I (idle)
 * Pid:	47938
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47939 ===
 * Name:	kworker/u36:3-ttm
 * State:	I (idle)
 * Pid:	47939
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47940 ===
 * Name:	kworker/u36:7-ttm
 * State:	I (idle)
 * Pid:	47940
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47941 ===
 * Name:	kworker/u36:8-ttm
 * State:	I (idle)
 * Pid:	47941
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47942 ===
 * Name:	kworker/u36:11-ttm
 * State:	I (idle)
 * Pid:	47942
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47943 ===
 * Name:	kworker/u36:16
 * State:	I (idle)
 * Pid:	47943
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 47944 ===
 * Name:	brave
 * State:	S (sleeping)
 * Pid:	47944
 * PPid:	16880
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	1518397880 kB
 * VmRSS:	   64720 kB
 * Threads:	9
 * Cmdline: /opt/brave-bin/brave --type=renderer --crashpad-handler-pid=16865 --enable-crash-reporter=b426e45e-c59a-4a94-809f-841e1b408ee9, --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --change-stack-guard-on-fork=enable --ozone-platform=wayland --lang=en-US --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=493 --launch-time-ticks=33168259193 --shared-files=v8_context_snapshot_data:100 --metrics-shmem-handle=4,i,5234558390566932709,16145285334683874320,1572864 --field-trial-handle=3,i,335299326240574126,7251934842164038653,262144 --enable-features=WaylandSessionManagement --variations-seed-version=main@6ff3baf661e484ef285861bc399c1df5322d04f8 --pseudonymization-salt-handle=7,i,1104738333880589867,4378977615861193026,4 --trace-process-track-uuid=3190709448273503051 
 *
 * === Process 47973 ===
 * Name:	kworker/u33:1-btrfs-endio
 * State:	I (idle)
 * Pid:	47973
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48078 ===
 * Name:	systemd-userwor
 * State:	S (sleeping)
 * Pid:	48078
 * PPid:	381
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    9780 kB
 * VmRSS:	    5484 kB
 * Threads:	1
 * Cmdline: systemd-userwork: waiting... 
 *
 * === Process 48079 ===
 * Name:	kworker/3:2
 * State:	I (idle)
 * Pid:	48079
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48097 ===
 * Name:	kworker/u34:2-btrfs-endio
 * State:	I (idle)
 * Pid:	48097
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48098 ===
 * Name:	kworker/u34:9-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	48098
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48099 ===
 * Name:	kworker/u34:11
 * State:	I (idle)
 * Pid:	48099
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48100 ===
 * Name:	kworker/u34:15-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	48100
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48102 ===
 * Name:	kworker/u33:4-btrfs-endio-write
 * State:	I (idle)
 * Pid:	48102
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48103 ===
 * Name:	kworker/u33:11-btrfs-endio-meta
 * State:	I (idle)
 * Pid:	48103
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48107 ===
 * Name:	systemd-userwor
 * State:	S (sleeping)
 * Pid:	48107
 * PPid:	381
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    9780 kB
 * VmRSS:	    5536 kB
 * Threads:	1
 * Cmdline: systemd-userwork: waiting... 
 *
 * === Process 48108 ===
 * Name:	systemd-userwor
 * State:	S (sleeping)
 * Pid:	48108
 * PPid:	381
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * VmSize:	    9780 kB
 * VmRSS:	    5540 kB
 * Threads:	1
 * Cmdline: systemd-userwork: waiting... 
 *
 * === Process 48155 ===
 * Name:	kworker/7:1
 * State:	I (idle)
 * Pid:	48155
 * PPid:	2
 * Uid:	0	0	0	0
 * Gid:	0	0	0	0
 * Threads:	1
 * Cmdline: 
 *
 * === Process 48157 ===
 * Name:	python3
 * State:	S (sleeping)
 * Pid:	48157
 * PPid:	47247
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   21040 kB
 * VmRSS:	   15232 kB
 * Threads:	1
 * Cmdline: python3 - 
 *
 * === Process 48266 ===
 * Name:	cpptools-srv2
 * State:	S (sleeping)
 * Pid:	48266
 * PPid:	14481
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	   80076 kB
 * VmRSS:	   24336 kB
 * Threads:	10
 * Cmdline: /home/kss/.vscode/extensions/ms-vscode.cpptools-1.34.4-linux-x64/bin/cpptools-srv2 -s {1832FC28-4DC4-4BB7-BFAC-D8F5A8E0D399} -p 46155 -i TagParser -t /tmp/.vscode-cpptools-1000/cpptools/crashes/97851425cb67bf4c7d0501c3f68d5437 
 *
 * === Process 48305 ===
 * Name:	ss_run_2_48157
 * State:	R (running)
 * Pid:	48305
 * PPid:	48157
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    3000 kB
 * VmRSS:	    2016 kB
 * Threads:	1
 * Cmdline: /tmp/ss_run_2_48157 
 *
 * === Process 48306 ===
 * Name:	cpuUsage.sh
 * State:	S (sleeping)
 * Pid:	48306
 * PPid:	44974
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    8044 kB
 * VmRSS:	    5844 kB
 * Threads:	1
 * Cmdline: /bin/bash /usr/share/code/resources/app/out/vs/base/node/cpuUsage.sh 47247 48157 48298 48303 48304 
 *
 * === Process 48307 ===
 * Name:	ss_run_2_48157
 * State:	S (sleeping)
 * Pid:	48307
 * PPid:	48305
 * Uid:	1000	1000	1000	1000
 * Gid:	1000	1000	1000	1000
 * VmSize:	    3000 kB
 * VmRSS:	    1436 kB
 * Threads:	1
 * Cmdline: /tmp/ss_run_2_48157 
 *
 * Child process terminated.
 */

