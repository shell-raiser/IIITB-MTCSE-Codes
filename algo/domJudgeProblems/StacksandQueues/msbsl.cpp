#include <iostream>
#include <vector>
#include <deque>

using namespace std;

// Among all non-empty contiguous subarrays of length at most k, find the
// greatest length of a subarray having the maximum possible sum.
//
// Let prefix[i] = sum of a[0..i-1], with prefix[0] = 0. A subarray a[i..j-1]
// has sum prefix[j] - prefix[i] and length j - i. For a fixed right endpoint
// j, we want to maximize prefix[j] - prefix[i] over i in [j-k, j-1], i.e.
// minimize prefix[i] in that window. Among indices achieving that minimum we
// want the earliest i (to maximize length).
//
// A monotonic deque of candidate i's with non-decreasing prefix values does
// this in O(n). The front always holds the earliest index achieving the
// current window minimum. We use strict > when popping the back so that
// equal prefix values are retained (preserving the earliest minimum).
//
// Sums may reach |a[i]| * n = 10^15 * 10^6 = 10^21, exceeding int64, so we
// use 128-bit arithmetic internally as the problem note suggests.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;
    vector<long long> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    vector<__int128> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + a[i];
    }

    deque<int> dq; // candidate left indices i, prefix[dq.front()] is the window min
    __int128 maxSum = 0;
    int bestLen = 0;
    bool first = true;

    for (int j = 1; j <= n; ++j) {
        // Insert i = j - 1 into the deque (it enters the window for endpoint j)
        int i = j - 1;
        while (!dq.empty() && prefix[dq.back()] > prefix[i]) {
            dq.pop_back();
        }
        dq.push_back(i);

        // Drop indices that fell out of the length-k window: i >= j - k
        while (!dq.empty() && dq.front() < j - k) {
            dq.pop_front();
        }

        int bestI = dq.front();
        __int128 sum = prefix[j] - prefix[bestI];
        int len = j - bestI;

        if (first || sum > maxSum) {
            maxSum = sum;
            bestLen = len;
            first = false;
        } else if (sum == maxSum && len > bestLen) {
            bestLen = len;
        }
    }

    cout << bestLen << "\n";
    return 0;
}