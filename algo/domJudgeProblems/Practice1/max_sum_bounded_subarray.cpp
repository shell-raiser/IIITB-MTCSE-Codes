#include <bits/stdc++.h>
using namespace std;

/**
 * Maximum-Sum Bounded Subarray Length
 * Given a sequence of n integers and an integer k, consider all non-empty contiguous subarrays
 * whose length is at most k. Find the length of a subarray having the maximum possible sum.
 * If several eligible subarrays have the same maximum sum, print the greatest length among them.
 *
 * Solution:
 * Use prefix sums: prefix[0] = 0, prefix[i] = sum of first i elements (a[0]..a[i-1]).
 * For each right endpoint r (1..n), we want a left endpoint l in [max(0, r-k), r-1] that
 * minimizes prefix[l] (to maximize sum = prefix[r] - prefix[l]).
 * Maintain a monotonic deque of candidate indices with increasing prefix values.
 * For each r, remove indices < r-k from front, then the best l is deque.front().
 * Update answer with sum and length (r - l) using tie-breaking rule.
 * Then insert index r into deque while preserving increasing prefix.
 *
 * Use __int128 for prefix sums to avoid overflow (constraints allow sums beyond 64-bit).
 *
 * Time Complexity: O(n)
 * Space Complexity: O(k) (deque size at most k)
 */

long long maxSumBoundedSubarrayLength(const vector<long long>& a, int k) {
    int n = a.size();
    vector<__int128> prefix(n+1, 0);
    for (int i = 0; i < n; ++i)
        prefix[i+1] = prefix[i] + a[i];

    deque<int> dq; // stores indices of prefix with increasing values
    dq.push_back(0); // prefix[0] = 0

    __int128 bestSum = numeric_limits<__int128>::min();
    long long bestLen = 0;

    for (int r = 1; r <= n; ++r) {
        // Remove indices out of window [r-k, r-1]
        while (!dq.empty() && dq.front() < r - k)
            dq.pop_front();

        if (!dq.empty()) {
            int l = dq.front();
            __int128 curSum = prefix[r] - prefix[l];
            long long curLen = r - l;
            if (curSum > bestSum || (curSum == bestSum && curLen > bestLen)) {
                bestSum = curSum;
                bestLen = curLen;
            }
        }

        // Insert r, maintaining increasing prefix values
        while (!dq.empty() && prefix[dq.back()] >= prefix[r])
            dq.pop_back();
        dq.push_back(r);
    }
    return bestLen;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    if (!(cin >> n >> k)) return 0;
    vector<long long> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];
    cout << maxSumBoundedSubarrayLength(a, k) << '\n';
    return 0;
}