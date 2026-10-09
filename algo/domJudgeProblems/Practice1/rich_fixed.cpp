#include <bits/stdc++.h>
using namespace std;

/**
 * RICH
 * Given array wealth of n positive integers and integer k (1 < k < n).
 * A person i is rich if count of people with wealth > k*wealth[i] <= k.
 *
 * Solution:
 * Sort wealth. For each w, threshold = k*w. Count elements > threshold
 * using upper_bound. If count <= k, person is rich. Use 128-bit for threshold.
 *
 * Time Complexity: O(n log n)
 * Space Complexity: O(1) extra (sort in-place or copy)
 */

long long countRich(const vector<long long>& wealth, long long k) {
    int n = wealth.size();
    vector<long long> w = wealth;
    sort(w.begin(), w.end());
    long long ans = 0;
    for (long long val : w) {
        __int128 thresh = (__int128)k * val;
        // count elements > thresh in sorted array w
        auto it = upper_bound(w.begin(), w.end(), (long long)thresh);
        long long cnt = w.end() - it;
        if (cnt <= k) ans++;
    }
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n; long long k;
    if (!(cin >> n >> k)) return 0;
    vector<long long> wealth(n);
    for (int i = 0; i < n; ++i) cin >> wealth[i];
    cout << countRich(wealth, k) << '\n';
    return 0;
}