#include <iostream>

using namespace std;

// O(n) mathematical recurrence for the Josephus problem (0-indexed).
// f(1) = 0; f(i) = (f(i-1) + k) % i
// The naive O(n*k) deque simulation TLEs for n,k up to 10^7.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n, k;
    cin >> n >> k;

    long long ans = 0; // f(1) = 0
    for (long long i = 2; i <= n; ++i) {
        ans = (ans + k) % i;
    }

    cout << ans << "\n";
    return 0;
}