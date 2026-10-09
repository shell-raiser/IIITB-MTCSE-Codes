#include <iostream>
#include <string>
#include <queue>

using namespace std;

class Solution {
public:
    string predictPartyVictory(string senate) {
        queue<int> R, D;
        int n = senate.size();

        // Build the queues
        for (int i = 0; i < n; i++) {
            if (senate[i] == 'R') {
                R.push(i);
            } else {
                D.push(i);
            }
        }

        while (!R.empty() && !D.empty()) {
            int rTurn = R.front(); R.pop();
            int dTurn = D.front(); D.pop();

            if (rTurn < dTurn) {
                R.push(rTurn + n); // adding offset so a later D can loop around and remove the initial Rs. The offset logic is enough and no need for mod shenanigans
            } else {
                D.push(dTurn + n);
            }
        }

        return R.empty() ? "Dire" : "Radiant";
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string senate;
    cin >> senate;

    Solution sol;
    cout << sol.predictPartyVictory(senate) << "\n";
    return 0;
}