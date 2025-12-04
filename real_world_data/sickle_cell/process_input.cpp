#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

using namespace std;

void process_input() {
    ifstream s_file("hg38_100kb.in");
    ifstream t_file("hbb_wt.in");

    int n, m;
    s_file >> n;
    t_file >> m;

    string s, t;
    for (int i = 0; i < 2000; i++) {
        string line;
        s_file >> line;
        s += line;
    }
    t_file >> t;

    assert(s.size() == n);
    assert(t.size() == m);

    ofstream out("input.in");
    out << n << " " << m << "\n";
    out << s << "\n" << t << "\n";

    // cout << s << "\n" << t << "\n";
    out.close();
}

int main() {
    process_input();
    return 0;
}
