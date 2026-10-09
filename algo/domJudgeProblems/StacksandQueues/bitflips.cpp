#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class Solution {
public:
    int minKBitFlips(vector<int>& nums, int k) {
        // Queue to keep track of flips within the window of k
        deque<int> q;
        // Current flip state
        int flipped = 0;
        // Total number of flips
        int result = 0;

        for (int i = 0; i < nums.size(); ++i) {
            // Remove the effect of the oldest flip(s) if it's out of the current window
            while (!q.empty() && i > q.front() + k - 1) {
                q.pop_front();
            }

            // If the current bit is 0 it needs to be flipped
            if ((nums[i] + q.size()) % 2 == 0) { // checking out the no. of flips performed on that index

                // If we cannot flip a subarray starting at index i
                if (i + k > nums.size()) {
                    return -1;
                }

                // Add a flip at this position
                q.push_back(i);
                // Increment the flip count
                result += 1;
            }
        }

        return result;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    cin >> n >> k;
    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    Solution sol;
    cout << sol.minKBitFlips(nums, k) << "\n";
    return 0;
}