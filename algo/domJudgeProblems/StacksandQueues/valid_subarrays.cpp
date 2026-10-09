#include <iostream>
#include <vector>
#include <stack>

using namespace std;

// A subarray nums[i..j] is valid if nums[i] is the minimum of that subarray.
// For each starting index i, let nextSmaller[i] be the first index j > i
// with nums[j] < nums[i] (or n if none). Then every subarray starting at i
// and ending before nextSmaller[i] is valid, contributing (nextSmaller[i] - i).
// Ties are allowed: equal elements do NOT break validity, so we look for the
// next STRICTLY smaller element (pop while nums[st.top()] >= nums[i]).
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    vector<int> nextSmaller(n, n);
    stack<int> st; // indices to the right, increasing value (smallest on top)
    for (int i = n - 1; i >= 0; --i) {
        while (!st.empty() && nums[st.top()] >= nums[i]) {
            st.pop();
        }
        nextSmaller[i] = st.empty() ? n : st.top();
        st.push(i);
    }

    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        ans += (nextSmaller[i] - i);
    }

    cout << ans << "\n";
    return 0;
}