#include <bits/stdc++.h>
using namespace std;

#include <immintrin.h>
#pragma GCC target "avx2"
#pragma GCC optimize "Ofast"

typedef __m256i m256;
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

#define vload(ptr) _mm256_loadu_si256((m256*) (ptr))
#define vstore(ptr, v) _mm256_storeu_si256((m256*) (ptr), (v))

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
        int ptr = 0;

        constexpr int BSZ = 32*255*4;
        FR(blk, (m+BSZ-1)/BSZ){
            m256 vRes1 = _mm256_setzero_si256();
            m256 vRes2 = _mm256_setzero_si256();
            m256 vRes3 = _mm256_setzero_si256();
            m256 vRes4 = _mm256_setzero_si256();

            int numIts = min(255, (m-ptr)/128);
            FR(j, numIts){
                m256 a1 = vload(s1 + i + ptr + 0);
                m256 a2 = vload(s1 + i + ptr + 32);
                m256 a3 = vload(s1 + i + ptr + 64);
                m256 a4 = vload(s1 + i + ptr + 96);
                m256 b1 = vload(s2 + ptr + 0);
                m256 b2 = vload(s2 + ptr + 32);
                m256 b3 = vload(s2 + ptr + 64);
                m256 b4 = vload(s2 + ptr + 96);
                vRes1 = _mm256_add_epi8(vRes1, _mm256_cmpeq_epi8(a1, b1));
                vRes2 = _mm256_add_epi8(vRes2, _mm256_cmpeq_epi8(a2, b2));
                vRes3 = _mm256_add_epi8(vRes3, _mm256_cmpeq_epi8(a3, b3));
                vRes4 = _mm256_add_epi8(vRes4, _mm256_cmpeq_epi8(a4, b4));
                ptr += 128;
            }

            uint8_t resArr[4][32];
            vstore(resArr[0], vRes1);
            vstore(resArr[1], vRes2);
            vstore(resArr[2], vRes3);
            vstore(resArr[3], vRes4);
            FR(a, 4) FR(b, 32) res += (uint8_t) (numIts + resArr[a][b]);
        }

        while(ptr < m) res += s1[i+ptr] != s2[ptr], ptr++;
        ans[i] = res;
    }

    FR(i, n-m+1) printf("%d%c", ans[i], " \n"[i == n-m]);
}
