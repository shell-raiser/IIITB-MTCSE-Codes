#include <bits/stdc++.h>
using namespace std();

/**
 * Good Index Pairs
 * Given two integer arrays A and B each of length n, count the number of index pairs (i, j)
 * such that 0 <= i < j < n and A[i] < B[j].
 *
 * Solution:
 * Process j from 0 to n-1. For each j, we need the count of i < j with A[i] < B[j].
 * Maintain a Fenwick Tree (Binary Indexed Tree) over the values of A[0..j-1] to query
 * prefix sums of frequencies. Since values can be up to 1e15, we compress all values
 * from A and B into a sorted vector.
 *
 * Steps:
 *   1. Collect all values from A and B, sort and unique -> compressed coordinates.
 *   2. Initialize Fenwick tree of size = number of unique values.
 *   3. Iterate j from 0 to n-1:
 *        - Query fenwick for sum of frequencies of values < B[j] (i.e., prefix up to idx(B[j])-1).
 *        - Add this count to answer.
 *        - Update fenwick at position of A[j] (add 1) to include this A[i] for future j.
 *   4. Output answer.
 *
 * Time Complexity: O(n log n)
 * Space Complexity: O(n)
 */

struct Fenwick {
    int n;
    vector<long long> bit;
    Fenwick(int n): n(n), bit(n+1, 0) {}
    void add(int idx, long long delta) {
        for (++idx; idx <= n; idx += idx & -idx)
            bit[idx] += delta;
    }
    long long sum(int idx) { // sum[0..idx]
        long long res = 0;
        for (++idx; idx > 0; idx -= idx & -idx)
            res += bit[idx];
        return res;
    }
    long long range_sum(int l, int r) { // sum[l..r]
        if (l > r) return 0;
        return sum(r) - (l ? sum(l-1) : 0);
    }
};

long long goodIndexPairs(const vector<long long>& A, const vector<long long>& B) {
    int n = A.size();
    // coordinate compression
    vector<long long> vals = A;
    vals.insert(vals.end(), B.begin(), B.end());
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    auto getIdx = [&](long long x) {
        return lower_bound(vals.begin(), vals.end(), x) - vals.begin();
    };
    Fenwick ft((int)vals.size());
    long long ans = 0;
    for (int j = 0; j < n; ++j) {
        // count of A[i] (i<j) with A[i] < B[j]
        int idxB = getIdx(B[j]);
        if (idxB > 0)
            ans += ft.sum(idxB-1);
        // add A[j] for future queries
        ft.add(getIdx(A[j]), 1);
    }
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    if (!(cin >> n)) return 0;
    vector<long long> A(n), B(n);
    for (int i = 0; i < n; ++i) cin >> A[i];
    for (int i = 0; i < n; ++i) cin >> B[i];
    cout << goodIndexPairs(A, B) << '\n';
    return 0;
}