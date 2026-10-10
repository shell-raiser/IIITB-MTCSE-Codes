#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

class Solution {
public:
    int maxPerformance(int n, vector<int>& speed, vector<int>& efficiency, int k) {
        constexpr int MOD = 1'000'000'007;
        vector<pair<int, int>> engineers;

        for (int i = 0; i < n; i++) {
            engineers.emplace_back(efficiency[i], speed[i]);
        }

        sort(engineers.rbegin(), engineers.rend());

        priority_queue<int, vector<int>, greater<int>> minHeap;
        long long speedSum = 0, res = 0;

        for (const auto& [eff, spd] : engineers) {
            if (minHeap.size() == k) {
                speedSum -= minHeap.top();
                minHeap.pop();
            }
            speedSum += spd;
            minHeap.push(spd);
            res = max(res, speedSum * eff);
        }

        return res % MOD;
    }
};

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, k;
    if (!(cin >> n >> k)) return 0;

    vector<int> speed(n);
    vector<int> efficiency(n);

    for (int i = 0; i < n; i++) {
        cin >> speed[i] >> efficiency[i];
    }

    Solution solver;
    cout << solver.maxPerformance(n, speed, efficiency, k) << "\n";

    return 0;
}