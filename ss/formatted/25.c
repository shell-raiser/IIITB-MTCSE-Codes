/*
25. Executing an Executable Program:
a. Execute a program using the exec system call.
b. Pass input to an executable program (e.g., execute an executable as $./a.out name).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program_to_execute> [args...]\n", argv[0]);
        printf("Example: %s ./a.out name\n", argv[0]);
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child - execute the program
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    } else {
        // Parent - wait for child
        wait(NULL);
        printf("Child process completed\n");
    }
    return 0;
}
/*
 * Output
 * Command: gcc 25.c -o 25
 *
25 ls
10.c   27.c   42b.c  54.c   9g.c               auto_file_4_2.txt
11a.c  28.c   42c.c  55.c   9h.c               auto_file_4_3.txt
11b.c  29.c   42d.c  56.c   9i.c               auto_file_4_4.txt
11c.c  2.c    42e.c  57a.c  9j.c               auto_file_5_0.txt
12.c   30.c   42f.c  57b.c  auto_file_0_0.txt  auto_file_5_1.txt
13.c   31.c   42g.c  57c.c  auto_file_0_1.txt  auto_file_5_2.txt
14.c   32.c   42h.c  58.c   auto_file_0_2.txt  auto_file_5_3.txt
15.c   33.c   43.c   59.c   auto_file_0_3.txt  auto_file_5_4.txt
16a.c  34.c   44.c   5.c    auto_file_0_4.txt  hardlink.txt
16b.c  35.c   45.c   60a.c  auto_file_1_0.txt  myfifo_mkfifo
17a.c  36a.c  46.c   60b.c  auto_file_1_1.txt  myfifo_mknod
17b.c  36b.c  47a.c  61a.c  auto_file_1_2.txt  myfile.txt
18a.c  36c.c  47b.c  61b.c  auto_file_1_3.txt  newfile.txt
18b.c  36d.c  47c.c  62a.c  auto_file_1_4.txt  output.txt
19.c   36e.c  47d.c  62b.c  auto_file_2_0.txt  README.txt
1a.c   37.c   48.c   63.c   auto_file_2_1.txt  seek_test.txt
1b.c   38a    49a.c  64.c   auto_file_2_2.txt  softlink.txt
1c.c   38a.c  49b.c  6.c    auto_file_2_3.txt  target.txt
20.c   38b    49c.c  7.c    auto_file_2_4.txt  test_dup2.txt
21.c   38b.c  49d.c  8.c    auto_file_3_0.txt  test_dup.txt
22.c   38c.c  4.c    9a.c   auto_file_3_1.txt  test_fcntl.txt
23.c   39.c   50.c   9b.c   auto_file_3_2.txt  testfile.txt
24.c   3.c    51.c   9c.c   auto_file_3_3.txt
25     40.c   52a.c  9d.c   auto_file_3_4.txt
25.c   41.c   52b.c  9e.c   auto_file_4_0.txt
26.c   42a.c  53.c   9f.c   auto_file_4_1.txt
Child process completed
 */

