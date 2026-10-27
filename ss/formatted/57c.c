/*
57. System Limitation Exploration
 c. Number of clock ticks (jiffies) per second.
*/

#include <stdio.h>
#include <unistd.h>

int main(void) {
    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck == -1) {
        perror("sysconf(_SC_CLK_TCK)");
        return 1;
    }

    printf("c. CLK_TCK (clock ticks per second): %ld\n", clk_tck);
    printf("   1 jiffy = %.6f seconds (%.6f ms)\n",
           1.0 / (double)clk_tck, 1000.0 / (double)clk_tck);

    return 0;
}
/*
 * Output
 * Command: gcc 57c.c -o 57c && ./57c
 *
 * c. CLK_TCK (clock ticks per second): 100
 *    1 jiffy = 0.010000 seconds (10.000000 ms)
 */
