/*
18. Record Locking Implementation: Write a program to perform record locking with the following implementations:
a. Implement a write lock.
b. Implement a read lock.
Create three records in a file. Whenever you access a particular record, first lock it, then modify/access it to avoid race conditions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

#define RECORD_SIZE 32
#define NUM_RECORDS 3

struct record {
    int id;
    char data[28];
};

void init_records() {
    int fd = open("records.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); exit(1); }
    
    struct record recs[NUM_RECORDS];
    for (int i = 0; i < NUM_RECORDS; i++) {
        recs[i].id = i + 1;
        snprintf(recs[i].data, sizeof(recs[i].data), "Record %d data", i + 1);
    }
    
    write(fd, recs, sizeof(recs));
    close(fd);
    printf("Initialized %d records\n", NUM_RECORDS);
}

void write_lock_record(int fd, int rec_num) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = rec_num * RECORD_SIZE;
    lock.l_len = RECORD_SIZE;
    
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("write lock failed");
        exit(1);
    }
    printf("Write lock acquired on record %d\n", rec_num + 1);
}

void read_lock_record(int fd, int rec_num) {
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = rec_num * RECORD_SIZE;
    lock.l_len = RECORD_SIZE;
    
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("read lock failed");
        exit(1);
    }
    printf("Read lock acquired on record %d\n", rec_num + 1);
}

void unlock_record(int fd, int rec_num) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = rec_num * RECORD_SIZE;
    lock.l_len = RECORD_SIZE;
    
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("unlock failed");
    } else {
        printf("Lock released on record %d\n", rec_num + 1);
    }
}

void write_record(int fd, int rec_num, const char *data) {
    write_lock_record(fd, rec_num);
    
    struct record rec;
    lseek(fd, rec_num * RECORD_SIZE, SEEK_SET);
    read(fd, &rec, sizeof(rec));
    
    strncpy(rec.data, data, sizeof(rec.data) - 1);
    rec.data[sizeof(rec.data) - 1] = '\0';
    
    lseek(fd, rec_num * RECORD_SIZE, SEEK_SET);
    write(fd, &rec, sizeof(rec));
    
    printf("Record %d updated: %s\n", rec_num + 1, rec.data);
    unlock_record(fd, rec_num);
}

void read_record(int fd, int rec_num) {
    read_lock_record(fd, rec_num);
    
    struct record rec;
    lseek(fd, rec_num * RECORD_SIZE, SEEK_SET);
    read(fd, &rec, sizeof(rec));
    
    printf("Record %d: ID=%d, Data='%s'\n", rec_num + 1, rec.id, rec.data);
    unlock_record(fd, rec_num);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <init|write|read> [record_num] [data]\n", argv[0]);
        printf("  init                    - Initialize records file\n");
        printf("  write <rec_num> <data>  - Write lock and update record\n");
        printf("  read <rec_num>          - Read lock and read record\n");
        exit(1);
    }
    
    if (strcmp(argv[1], "init") == 0) {
        init_records();
        return 0;
    }
    
    int fd = open("records.dat", O_RDWR);
    if (fd < 0) {
        perror("open failed (run init first)");
        exit(1);
    }
    
    if (strcmp(argv[1], "write") == 0) {
        if (argc < 4) {
            printf("Usage: %s write <rec_num> <data>\n", argv[0]);
            close(fd);
            exit(1);
        }
        int rec_num = atoi(argv[2]) - 1;
        if (rec_num < 0 || rec_num >= NUM_RECORDS) {
            printf("Record number must be 1-%d\n", NUM_RECORDS);
            close(fd);
            exit(1);
        }
        write_record(fd, rec_num, argv[3]);
    } else if (strcmp(argv[1], "read") == 0) {
        if (argc < 3) {
            printf("Usage: %s read <rec_num>\n", argv[0]);
            close(fd);
            exit(1);
        }
        int rec_num = atoi(argv[2]) - 1;
        if (rec_num < 0 || rec_num >= NUM_RECORDS) {
            printf("Record number must be 1-%d\n", NUM_RECORDS);
            close(fd);
            exit(1);
        }
        read_record(fd, rec_num);
    } else {
        printf("Invalid command\n");
    }
    
    close(fd);
    return 0;
}
/*
 * Output
 * Command: gcc 18b.c -o 18b
 *
 * Usage: /tmp/ss_run_18b_48157 <init|write|read> [record_num] [data]
 *   init                    - Initialize records file
 *   write <rec_num> <data>  - Write lock and update record
 *   read <rec_num>          - Read lock and read record
 */

