#include <bits/stdc++.h>
using namespace std;

#pragma GCC target "avx2"
#pragma GCC optimize "Ofast"

typedef unsigned uint;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef vector<int> vi;
typedef vector<pii> vpii;
typedef complex<double> cpx;
template <typename T> using minPq = priority_queue<T, vector<T>, greater<T>>;
#define ms(a, x) memset(a, x, sizeof(a))
#define pb push_back
#define fs first
#define sn second
#define ALL(v) begin(v), end(v)
#define SZ(v) ((int) (v).size())
#define lbv(v, x) (lower_bound(ALL(v), x) - (v).begin())
#define ubv(v, x) (upper_bound(ALL(v), x) - (v).begin())
template <typename T> inline void UNIQUE(vector<T> &v){sort(ALL(v)); v.resize(unique(ALL(v)) - v.begin());}
const int INF = 0x3f3f3f3f;
const ll LLINF = 0x3f3f3f3f3f3f3f3f;
const double PI = acos(-1);
#define FR(i, n) for(int i = 0; i < (n); i++)
#define FOR(i, a, b) for(int i = (a); i < (b); i++)
#define FORR(i, a, b) for(int i = (a); i >= (b); i--)
#define dbg(x) {cerr << #x << ' ' << x << endl;}
#define dbgArr(arr, n) {cerr << #arr; FR(_i, n) cerr << ' ' << (arr)[_i]; cerr << endl;}
template <typename T, typename U>
ostream& operator<<(ostream &os, pair<T, U> p){return os << "(" << p.fs << ", " << p.sn << ")";}

const int MN = 10 << 20;
int n, m;
char s1[MN], s2[MN];
int ans[MN];

int main(){
    scanf("%d %d\n", &n, &m);
    assert(n < MN && m <= n);
    fread(s1, 1, n+1, stdin);
    fread(s2, 1, m+1, stdin);
    s1[n] = s2[m] = 0;

    FR(i, n-m+1){
        int res = 0;
        FR(j, m) res += s1[i+j] != s2[j];
        ans[i] = res;
    }

    FR(i, n-m+1) printf("%d%c", ans[i], " \n"[i == n-m]);
}
