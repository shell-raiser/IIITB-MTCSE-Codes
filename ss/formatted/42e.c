/*
42. Message Queue Information: Write a program to print information about a message queue using msqid_ds and ipc_perm structures:
a. Access permission
b. UID, GID
c. Time of last message sent and received
d. Time of last change in the message queue
e. Size of the queue
f. Number of messages in the queue
g. Maximum number of bytes allowed
h. PID of the msgsnd and msgrcv
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <message_queue_id>\n", argv[0]);
        printf("Get ID from Exercise 41 or ipcs -q\n");
        exit(1);
    }
    
    int msgid = atoi(argv[1]);
    struct msqid_ds buf;
    
    if (msgctl(msgid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT failed");
        exit(1);
    }
    
    printf("Message Queue Information (ID: %d):\n", msgid);
    printf("========================================\n");
    
    // a. Access permission
    printf("a. Access permissions: %o\n", buf.msg_perm.mode & 0777);
    
    // b. UID, GID
    printf("b. Owner UID: %d, GID: %d\n", buf.msg_perm.uid, buf.msg_perm.gid);
    printf("   Creator UID: %d, GID: %d\n", buf.msg_perm.cuid, buf.msg_perm.cgid);
    
    // c. Time of last message sent and received
    printf("c. Last msgsnd time: %s", ctime(&buf.msg_stime));
    printf("   Last msgrcv time: %s", ctime(&buf.msg_rtime));
    
    // d. Time of last change
    printf("d. Last change time: %s", ctime(&buf.msg_ctime));
    
    // e. Size of the queue (current bytes)
    printf("e. Current queue size: %lu bytes\n", buf.msg_cbytes);
    
    // f. Number of messages in the queue
    printf("f. Number of messages: %lu\n", buf.msg_qnum);
    
    // g. Maximum number of bytes allowed
    printf("g. Max queue size: %lu bytes\n", buf.msg_qbytes);
    
    // h. PID of last msgsnd and msgrcv
    printf("h. Last sender PID: %d\n", buf.msg_lspid);
    printf("   Last receiver PID: %d\n", buf.msg_lrpid);
    
    return 0;
}

/*
 * Sample output for 42e.c
 * Example runtime:
 * $ gcc 42e.c -o 42e
 * $ ./42e
 *
 * Current queue size: 0 bytes
 */

/*
 * Actual output of 42e.c
 * Command: gcc 42e.c -o 42e
 *
 * Usage: /tmp/ss_run_42e_48157 <message_queue_id>
 * Get ID from Exercise 41 or ipcs -q
 */

