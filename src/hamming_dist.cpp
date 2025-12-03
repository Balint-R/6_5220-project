#include <bits/stdc++.h>
#include "/Users/mac/mylibrary/debug.hpp"

using namespace std;

vector<int> brute_force() {
    int n, m;
    cin >> n >> m;
    string s, t;
    cin >> s >> t;

    vector<int> dist(n - m + 1);
    for (int i = 0; i < n - m + 1; i++) {
        int cur_dist = 0;
        for (int j = 0; j < m; j++) {
            if (s[i + j] != t[j]) {
                cur_dist++;
            }
        }
        dist[i] = cur_dist;
    }
    return dist;
}


void print_vec(vector<int>& v) {
    for (int x : v)
        cout << x << ' ';
    cout << '\n';
}

int main() {

    freopen("../data/1.in", "r", stdin);
    vector<int> res = brute_force();
    print_vec(res);

    return 0;
}
