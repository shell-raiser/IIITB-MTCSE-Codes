#include <bits/stdc++.h>
using namespace std();

/**
 * Sum of Digits
 * Given a non-negative integer N, calculate the sum of all its digits.
 * 
 * Solution: Repeatedly extract last digit using modulo 10 and add to sum.
 * 
 * Time Complexity: O(log10 N) (number of digits)
 * Space Complexity: O(1)
 */

long long sumOfDigits(long long N) {
    long long sum = 0;
    while (N > 0) {
        sum += N % 10;
        N /= 10;
    }
    return sum;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    long long N;
    if (!(cin >> N)) return 0;
    cout << sumOfDigits(N) << '\n';
    return 0;
}