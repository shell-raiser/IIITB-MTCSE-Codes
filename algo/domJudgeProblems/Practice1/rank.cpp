#include <bits/stdc++.h>
using namespace std();

/**
 * Rank
 * Find the k-th largest element in the union of:
 *   S1 = even numbers in [a, b]
 *   S2 = multiples of 3 in [c, d]
 * If k is out of range, output -1.
 *
 * Solution:
 * Let f(x) = number of elements in U that are >= x.
 * f(x) is monotonic decreasing. The k-th largest element is the smallest x
 * such that f(x) >= k. Use binary search on x in [1, max(b,d)].
 * Count of numbers in an AP [L,R] with step s:
 *   first = L + ((s - L%s) % s); if first > R => 0 else (R-first)/s + 1.
 *
 * Time Complexity: O(log(max(b,d))) ~ O(30)
 * Space Complexity: O(1)
 */

long long countAP(long long L, long long R, long long step) {
    if (L > R) return 0;
    long long first = L + ((step - L % step) % step);
    if (first > R) return 0;
    return (R - first) / step + 1;
}

long long countGE(long long x, long long a, long long b, long long c, long long d) {
    long long s1 = countAP(max(a, x), b, 2);
    long long s2 = countAP(max(c, x), d, 3);
    long long inter = countAP(max({a, c, x}), min(b, d), 6);
    return s1 + s2 - inter;
}

long long solve(long long a, long long b, long long c, long long d, long long k) {
    long long total = countAP(a, b, 2) + countAP(c, d, 3) - countAP(max(a, c), min(b, d), 6);
    if (k < 1 || k > total) return -1;

    long long lo = 1, hi = max(b, d);
    while (lo < hi) {
        long long mid = (lo + hi) / 2;
        if (countGE(mid, a, b, c, d) >= k)
            hi = mid;
        else
            lo = mid + 1;
    }
    return lo;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long a, b, c, d, k;
    if (!(cin >> a >> b >> c >> d >> k)) return 0;
    cout << solve(a, b, c, d, k) << '\n';
    return 0;
}