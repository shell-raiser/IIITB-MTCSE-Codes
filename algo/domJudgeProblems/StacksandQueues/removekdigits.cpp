#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    string removeKdigits(string num, int k) {
        // num is a non negative int
        // return smallest int after removing k digits from num

        // leftmost digit matters -> for A = 1axxx , B = 1bxxx , if the digits a > b, then A>B
        // As we process each digit, we pop from the stack whenever the top is larger than the current digit and we still have removals left.
        // This greedily ensures that smaller digits bubble up to the front.
        vector<char> st;
        for (char i : num) {
            while (!st.empty() && i < st.back() && k > 0) {
                st.pop_back();
                k--;
            }
            st.push_back(i);
        }

        while (k > 0 && !st.empty()) {
            st.pop_back();
            k--;
        }
        for (auto it = st.begin(); it != st.end(); ) {
            if (*it == '0') {
                it = st.erase(it);
            } else {
                break;
            }
        }
        if (st.empty()) {
            return "0";
        }
        string ans(st.begin(), st.end());
        return ans;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string num;
    int k;
    cin >> num >> k;

    Solution sol;
    cout << sol.removeKdigits(num, k) << "\n";
    return 0;
}