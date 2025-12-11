#include "ham_dist_bf_fast.h"
#include <algorithm>
#include <cassert>
using namespace std;

#define USE_AVX __has_include(<immintrin.h>)

#if USE_AVX
    #include <immintrin.h>
    #pragma GCC target "avx2"
    typedef __m256i m256;
    #define vload(ptr) _mm256_loadu_si256((m256*) (ptr))
    #define vstore(ptr, v) _mm256_storeu_si256((m256*) (ptr), (v))
#endif

typedef long long ll;

void ham_dist_bf_fast(int n, int m, int sigma, const vector<uint32_t> &A,
                       const vector<uint32_t> &B, vector<uint32_t> &result){
    int new_m = (m + 127)/128 * 128;
    // Number of matches
    vector<int> ans(n+m-1);
    vector<uint8_t> arr(n), brr(new_m);

    if(sigma > 255){
        vector<int> freq_a(sigma), ptrs_b(sigma+1);
        // Compute frequencies
        for(int i = 0; i < n; i++) freq_a[A[i]]++;
        for(int i = 0; i < m; i++) ptrs_b[B[i]]++;

        vector<ll> freqs(sigma);
        for(int i = 0; i < sigma; i++) freqs[i] = (ll) freq_a[i] * ptrs_b[i];
        nth_element(freqs.begin(), freqs.begin() + sigma - 255, freqs.end());
        ll cut = freqs[sigma - 255]; // Remove all characters with freq <= cut

        // Convert to offset pointers
        for(int i = 0; i < sigma; i++) ptrs_b[i+1] += ptrs_b[i];

        // Fill array with indices
        vector<int> b_inds(m);
        for(int i = 0; i < m; i++) b_inds[--ptrs_b[B[i]]] = i;

        for(int i = 0; i < n; i++){
            int ch = A[i];
            ll freq = (ll) freq_a[ch] * (ptrs_b[ch+1] - ptrs_b[ch]);
            if(freq > cut){
                arr[i] = ch;
                continue;
            }
            arr[i] = 254;

            for(int p = ptrs_b[ch]; p < ptrs_b[ch+1]; p++){
                int j = b_inds[p];
                ans[i-j+m-1]++;
            }
        }

        for(int i = 0; i < m; i++){
            int ch = B[i];
            ll freq = (ll) freq_a[ch] * (ptrs_b[ch+1] - ptrs_b[ch]);
            brr[i] = freq > cut ? ch : 255;
        }
        for(int i = m; i < new_m; i++) brr[i] = 255;
    }
    else {
        for(int i = 0; i < n; i++) arr[i] = A[i];
        for(int i = 0; i < m; i++) brr[i] = B[i];
        for(int i = m; i < new_m; i++) brr[i] = 255;
    }

    #if USE_AVX
        uint8_t *a_ptr = &arr[0];
        uint8_t *b_ptr = &brr[0];

        for(int i = 0; i < n-m+1; i++){
            int res = ans[i+m-1];
            int ptr = 0;

            constexpr int BSZ = 32*255*4;
            for(int blk = 0; blk < (new_m+BSZ-1)/BSZ; blk++){
                m256 vRes1 = _mm256_setzero_si256();
                m256 vRes2 = _mm256_setzero_si256();
                m256 vRes3 = _mm256_setzero_si256();
                m256 vRes4 = _mm256_setzero_si256();

                int numIts = min(255, (new_m-ptr)/128);
                for(int j = 0; j < numIts; j++){
                    m256 a1 = vload(a_ptr + i + ptr + 0);
                    m256 a2 = vload(a_ptr + i + ptr + 32);
                    m256 a3 = vload(a_ptr + i + ptr + 64);
                    m256 a4 = vload(a_ptr + i + ptr + 96);
                    m256 b1 = vload(b_ptr + ptr + 0);
                    m256 b2 = vload(b_ptr + ptr + 32);
                    m256 b3 = vload(b_ptr + ptr + 64);
                    m256 b4 = vload(b_ptr + ptr + 96);
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
                    res += (uint8_t) -resArr[a][b];
                }
            }

            assert(ptr == new_m);
            result[i] = m - res;
        }
    #else
        for(int i = 0; i < n-m+1; i++){
            int res = ans[i+m-1];
            for(int j = 0; j < m; j++) res += arr[i+j] == brr[j];
            result[i] = m - res;
        }
    #endif
}
