#include <bits/stdc++.h>
using namespace std;

#include <immintrin.h>
#pragma GCC target "avx2"
#pragma GCC optimize "Ofast"

typedef __m256i m256;

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

    for(int i = 0; i < n-m+1; i++){
        int res = 0;
        int ptr = 0;

        constexpr int BSZ = 32*255*4;
        for(int blk = 0; blk < (m+BSZ-1)/BSZ; blk++){
            m256 vRes1 = _mm256_setzero_si256();
            m256 vRes2 = _mm256_setzero_si256();
            m256 vRes3 = _mm256_setzero_si256();
            m256 vRes4 = _mm256_setzero_si256();

            int numIts = min(255, (m-ptr)/128);
            for(int j = 0; j < numIts; j++){
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
            for(int a = 0; a < 4; a++) for(int b = 0; b < 32; b++){
                res += (uint8_t) (numIts + resArr[a][b]);
            }
        }

        while(ptr < m) res += s1[i+ptr] != s2[ptr], ptr++;
        ans[i] = res;
    }

    for(int i = 0; i < n-m+1; i++) printf("%d%c", ans[i], " \n"[i == n-m]);
}
