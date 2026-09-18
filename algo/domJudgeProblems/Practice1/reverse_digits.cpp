#include <bits/stdc++.h>
using namespace std;

/**
 * Reverse (decimal digits)
 * Given a positive integer N (1 ≤ N < 10^18), reverse its decimal digits and
 * output the reversed integer without leading zeros.
 *
 * Solution: Extract digits from the end and build the reversed number.
 * Alternatively, convert to string, reverse, and remove leading zeros.
 * We'll use the arithmetic method to avoid string conversion.
 *
 * Time Complexity: O(log10 N) (number of digits)
 * Space Complexity: O(1)
 */

long long reverseDigits(long long N) {
    long long rev = 0;
    while (N > 0) {
        rev = rev * 10 + (N % 10);
        N /= 10;
    }
    return rev;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    long long N;
    if (!(cin >> N)) return 0;
    cout << reverseDigits(N) << '\n';
    return 0;
}