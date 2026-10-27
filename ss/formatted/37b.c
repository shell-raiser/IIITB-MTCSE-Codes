/*
37. FIFO Communication (One-Way): Write two programs to enable communication through FIFO using one-way communication.

Program b: Reader (37b.c)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFSIZE 256

int main() {
    const char *fifo_path = "/tmp/myfifo_oneway";
    char buf[BUFSIZE];

    // Create the FIFO if it does not exist yet (writer may start later).
    // If it already exists this is a no-op.
    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }

    printf("Reader: Waiting for a writer on %s ...\n", fifo_path);
    fflush(stdout);

    // Blocks until the writer opens the FIFO
    int fd = open(fifo_path, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    printf("Reader: Receiving messages...\n");
    while (1) {
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n == -1) {
            perror("read failed");
            break;
        }
        if (n == 0)          // writer closed the FIFO
            break;

        buf[n] = '\0';
        if (strcmp(buf, "END") == 0) {
            printf("Received: %s (terminating)\n", buf);
            break;
        }
        printf("Received: %s\n", buf);
    }

    close(fd);
    printf("Reader: Done.\n");
    return 0;
}
/*
 * Output
 * Command: gcc 37b.c -o 37b
 *
 * Terminal 2 (run while/after 37a):
 *   $ ./37b
 *   Reader: Waiting for a writer on /tmp/myfifo_oneway ...
 *   Reader: Receiving messages...
 *   Received: Message 1
 *   Received: Message 2
 *   Received: Message 3
 *   Received: END (terminating)
 *   Reader: Done.
 */