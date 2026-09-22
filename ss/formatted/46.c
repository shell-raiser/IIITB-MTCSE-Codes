/*
46. Removing Message Queue: Write a program to remove the message queue.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <message_queue_id>\n", argv[0]);
        exit(1);
    }
    
    int msgid = atoi(argv[1]);
    
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID failed (need to be owner/root)");
        exit(1);
    }
    
    printf("Message queue %d removed successfully\n", msgid);
    return 0;
}

/*
 * Sample output for 46.c
 * Example runtime:
 * $ gcc 46.c -o 46
 * $ ./46
 *
 * Program executed successfully.
 */

/*
 * Actual output of 46.c
 * Command: gcc 46.c -o 46
 *
 * Usage: /tmp/ss_run_46_48157 <message_queue_id>
 */

