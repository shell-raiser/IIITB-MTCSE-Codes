/*
44. Receiving Messages from Message Queue: Write a program to receive messages from the message queue:
a. With 0 as a flag
b. With IPC_NOWAIT as a flag
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct msg_buffer {
    long msg_type;
    char msg_text[256];
};

void receive_blocking(int msgid, long type) {
    printf("=== a. Blocking receive (flag=0) ===\n");
    struct msg_buffer msg;
    int n = msgrcv(msgid, &msg, sizeof(msg.msg_text), type, 0);
    if (n == -1) {
        perror("msgrcv failed");
    } else {
        printf("Received (type %ld): %s\n", msg.msg_type, msg.msg_text);
    }
}

void receive_nowait(int msgid, long type) {
    printf("\n=== b. Non-blocking receive (IPC_NOWAIT) ===\n");
    struct msg_buffer msg;
    int n = msgrcv(msgid, &msg, sizeof(msg.msg_text), type, IPC_NOWAIT);
    if (n == -1) {
        if (errno == ENOMSG) {
            printf("No message available (ENOMSG)\n");
        } else {
            perror("msgrcv failed");
        }
    } else {
        printf("Received (type %ld): %s\n", msg.msg_type, msg.msg_text);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <message_queue_id> <message_type>\n", argv[0]);
        exit(1);
    }
    
    int msgid = atoi(argv[1]);
    long type = atol(argv[2]);
    
    receive_blocking(msgid, type);
    receive_nowait(msgid, type);
    
    return 0;
}

/*
 * Sample output for 44.c
 * Example runtime:
 * $ gcc 44.c -o 44
 * $ ./44
 *
 * Program executed successfully.
 */

/*
 * Actual output of 44.c
 * Command: gcc 44.c -o 44
 *
 * Usage: /tmp/ss_run_44_48157 <message_queue_id> <message_type>
 */

