#include <bits/stdc++.h>
using namespace std;

/**
 * FIB (custom recurrence)
 * Define F(i) = i for i = 0..k
 * For i > k: F(i) = (F(i-1) + F(i-k)) mod m
 * Given n, m, k, compute F(n) mod m.
 *
 * Solution: Linear recurrence of order k:
 *   F(n) = F(n-1) + F(n-k)   (all modulo m)
 * Use matrix exponentiation of companion matrix size k.
 * State vector S_n = [F(n), F(n-1), ..., F(n-k+1)]^T.
 * Transition matrix M (k x k):
 *   M[0][0] = 1, M[0][k-1] = 1, and M[i][i-1] = 1 for i=1..k-1.
 * Then S_n = M^(n-k) * S_k for n > k, where S_k = [F(k),...,F(1)].
 * For n <= k, answer = n mod m.
 *
 * Time Complexity: O(k^3 log n)
 * Space Complexity: O(k^2)
 */

using Matrix = vector<vector<long long>>;

Matrix matMul(const Matrix& A, const Matrix& B, long long mod) {
    int k = A.size();
    Matrix C(k, vector<long long>(k, 0));
    for (int i = 0; i < k; ++i)
        for (int l = 0; l < k; ++l) if (A[i][l])
            for (int j = 0; j < k; ++j)
                C[i][j] = (C[i][j] + A[i][l] * B[l][j]) % mod;
    return C;
}

Matrix matPow(Matrix base, long long exp, long long mod) {
    int k = base.size();
    Matrix res(k, vector<long long>(k, 0));
    for (int i = 0; i < k; ++i) res[i][i] = 1;
    while (exp > 0) {
        if (exp & 1) res = matMul(res, base, mod);
        base = matMul(base, base, mod);
        exp >>= 1;
    }
    return res;
}

long long fibCustom(long long n, long long m, long long k) {
    if (n <= k) return n % m;
    // build transition matrix
    Matrix M(k, vector<long long>(k, 0));
    M[0][0] = 1 % m;
    M[0][k-1] = 1 % m;
    for (int i = 1; i < k; ++i)
        M[i][i-1] = 1 % m;
    // power = n - k
    Matrix Mp = matPow(M, n - k, m);
    // initial state S_k = [F(k), F(k-1), ..., F(1)] where F(i)=i mod m
    vector<long long> init(k);
    for (int i = 0; i < k; ++i)
        init[i] = (k - i) % m; // F(k-i) = k-i
    // compute S_n = Mp * init
    long long res = 0;
    for (int j = 0; j < k; ++j)
        res = (res + Mp[0][j] * init[j]) % m;
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    long long n, m, k;
    if (!(cin >> n >> m >> k)) return 0;
    cout << fibCustom(n, m, k) << '\n';
    return 0;
}