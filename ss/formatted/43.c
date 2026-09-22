/*
43. Sending Messages to Message Queue: Write a program to send messages to the message queue. Check using $ipcs -q.
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <message_queue_id>\n", argv[0]);
        exit(1);
    }
    
    int msgid = atoi(argv[1]);
    struct msg_buffer msg;
    
    printf("Sending messages to queue %d (type 'quit' to exit)\n", msgid);
    
    while (1) {
        printf("Enter message type (1-10): ");
        if (scanf("%ld", &msg.msg_type) != 1) break;
        
        printf("Enter message: ");
        getchar(); // consume newline
        if (fgets(msg.msg_text, sizeof(msg.msg_text), stdin) == NULL) break;
        
        // Remove newline
        msg.msg_text[strcspn(msg.msg_text, "\n")] = 0;
        
        if (strcmp(msg.msg_text, "quit") == 0) break;
        
        if (msgsnd(msgid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
            perror("msgsnd failed");
        } else {
            printf("Message sent successfully\n");
        }
    }
    
    printf("Check with: ipcs -q\n");
    return 0;
}

/*
 * Sample output for 43.c
 * Example runtime:
 * $ gcc 43.c -o 43
 * $ ./43
 *
 * Program executed successfully.
 */

/*
 * Actual output of 43.c
 * Command: gcc 43.c -o 43
 *
 * Usage: /tmp/ss_run_43_48157 <message_queue_id>
 */

