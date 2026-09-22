/*
45. Changing Message Queue Permissions: Write a program to change the existing message queue permissions using the msqid_ds structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <message_queue_id> <octal_permissions>\n", argv[0]);
        printf("Example: %s 12345 0644\n", argv[0]);
        exit(1);
    }
    
    int msgid = atoi(argv[1]);
    int new_perm = strtol(argv[2], NULL, 8);
    
    struct msqid_ds buf;
    
    // Get current permissions
    if (msgctl(msgid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT failed");
        exit(1);
    }
    
    printf("Current permissions: %o\n", buf.msg_perm.mode & 0777);
    printf("Current UID: %d, GID: %d\n", buf.msg_perm.uid, buf.msg_perm.gid);
    
    // Modify permissions
    buf.msg_perm.mode = (buf.msg_perm.mode & ~0777) | (new_perm & 0777);
    
    if (msgctl(msgid, IPC_SET, &buf) == -1) {
        perror("msgctl IPC_SET failed (need to be owner/root)");
        exit(1);
    }
    
    printf("Permissions changed to: %o\n", new_perm);
    
    // Verify
    if (msgctl(msgid, IPC_STAT, &buf) == 0) {
        printf("Verified permissions: %o\n", buf.msg_perm.mode & 0777);
    }
    
    return 0;
}
