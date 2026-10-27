/*
37. FIFO Communication (One-Way): Write two programs to enable communication through FIFO using one-way communication.

Program a: Writer (37a.c)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    const char *fifo_path = "/tmp/myfifo_oneway";
    const char *messages[] = {"Message 1", "Message 2", "Message 3", "END", NULL};

    // Create the FIFO; EEXIST is fine (left over from a previous run)
    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }

    printf("Writer: Waiting for a reader on %s ...\n", fifo_path);
    fflush(stdout);

    // Blocks until the reader opens the FIFO
    int fd = open(fifo_path, O_WRONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    printf("Writer: Sending messages...\n");
    for (int i = 0; messages[i] != NULL; i++) {
        if (write(fd, messages[i], strlen(messages[i]) + 1) == -1) {
            perror("write failed");
            close(fd);
            exit(1);
        }
        printf("Sent: %s\n", messages[i]);
        sleep(1);
    }

    close(fd);
    unlink(fifo_path);
    printf("Writer: Done, FIFO removed.\n");
    return 0;
}
/*
 * Output
 * Command: gcc 37a.c -o 37a
 *
 * Terminal 1:
 *   $ ./37a
 *   Writer: Waiting for a reader on /tmp/myfifo_oneway ...
 *   Writer: Sending messages...
 *   Sent: Message 1
 *   Sent: Message 2
 *   Sent: Message 3
 *   Sent: END
 *   Writer: Done, FIFO removed.
 */