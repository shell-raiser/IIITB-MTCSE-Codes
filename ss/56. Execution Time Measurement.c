/*
56. Execution Time Measurement: Write a program to measure the time taken to execute 100 getppid() system calls using a time stamp counter.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <x86intrin.h> // For __rdtsc on x86

// Read Time Stamp Counter (TSC)
static inline unsigned long long rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main() {
    unsigned long long start_tsc, end_tsc;
    struct timeval start_tv, end_tv;
    struct timespec start_ts, end_ts;
    pid_t ppid;
    int iterations = 100;
    
    printf("Measuring time for %d getppid() system calls\n", iterations);
    printf("==============================================\n");
    
    // Method 1: Time Stamp Counter (TSC)
    start_tsc = rdtsc();
    for (int i = 0; i < iterations; i++) {
        ppid = getppid();
    }
    end_tsc = rdtsc();
    printf("TSC cycles: %llu\n", end_tsc - start_tsc);
    printf("TSC per call: %.2f\n", (double)(end_tsc - start_tsc) / iterations);
    
    // Method 2: gettimeofday (microsecond resolution)
    gettimeofday(&start_tv, NULL);
    for (int i = 0; i < iterations; i++) {
        ppid = getppid();
    }
    gettimeofday(&end_tv, NULL);
    long us = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 + (end_tv.tv_usec - start_tv.tv_usec);
    printf("gettimeofday: %ld microseconds total, %.2f per call\n", us, (double)us / iterations);
    
    // Method 3: clock_gettime (nanosecond resolution)
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    for (int i = 0; i < iterations; i++) {
        ppid = getppid();
    }
    clock_gettime(CLOCK_MONOTONIC, &end_ts);
    long ns = (end_ts.tv_sec - start_ts.tv_sec) * 1000000000 + (end_ts.tv_nsec - start_ts.tv_nsec);
    printf("clock_gettime: %ld nanoseconds total, %.2f per call\n", ns, (double)ns / iterations);
    
    // Method 4: times() system call
    struct tms start_tms, end_tms;
    times(&start_tms);
    for (int i = 0; i < iterations; i++) {
        ppid = getppid();
    }
    times(&end_tms);
    long ticks = (end_tms.tms_utime + end_tms.tms_stime) - (start_tms.tms_utime + start_tms.tms_stime);
    long clk_tck = sysconf(_SC_CLK_TCK);
    printf("times(): %ld ticks total, %.2f per call (clk_tck=%ld)\n", ticks, (double)ticks / iterations, clk_tck);
    
    return 0;
}
