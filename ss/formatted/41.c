/*
41. Message Queue Creation: Write a program to create a message queue, and print the key and message queue ID.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main() {
    key_t key;
    int msgid;
    
    // Generate key using ftok
    key = ftok(".", 'A');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }
    printf("Generated key: 0x%x\n", key);
    
    // Create message queue
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }
    
    printf("Message queue ID: %d\n", msgid);
    printf("Key: 0x%x\n", key);
    
    // Also try with IPC_PRIVATE
    int msgid_private = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgid_private != -1) {
        printf("\nPrivate message queue ID: %d\n", msgid_private);
        msgctl(msgid_private, IPC_RMID, NULL); // Clean up
    }
    
    // Don't remove the first one so user can check with ipcs -q
    printf("\nCheck with: ipcs -q\n");
    printf("Remove with: ipcrm -q %d\n", msgid);
    
    return 0;
}

/*
 * Sample output for 41.c
 * Example runtime:
 * $ gcc 41.c -o 41
 * $ ./41
 *
 * Program executed successfully.
 */

/*
 * Actual output of 41.c
 * Command: gcc 41.c -o 41
 *
 * Generated key: 0x413664f1
 * Message queue ID: 0
 * Key: 0x413664f1
 *
 * Private message queue ID: 1
 *
 * Check with: ipcs -q
 * Remove with: ipcrm -q 0
 */

