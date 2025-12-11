#include "ham_dist_sqrt.h"
#include "atcoder/convolution.hpp"
#include <cmath>
#include <cstdio>

using namespace std;

typedef long long ll;

void ham_dist_sqrt(int n, int m, int sigma,
                   const vector<uint32_t> &A, const vector<uint32_t> &B, 
                   vector<uint32_t> &result) {
    // Number of matches
    vector<int> ans(n+m-1);
    vector<int> freq_a(sigma), ptrs_b(sigma+1);
    // Compute frequencies
    for(int i = 0; i < n; i++) freq_a[A[i]]++;
    for(int i = 0; i < m; i++) ptrs_b[B[i]]++;

    // Convert to offset pointers
    for(int i = 0; i < sigma; i++) ptrs_b[i+1] += ptrs_b[i];

    // Fill array with indices
    vector<int> b_inds(m);
    for(int i = 0; i < m; i++) b_inds[--ptrs_b[B[i]]] = i;

    int cut = n * log2(n) * 8;
    if(n >= 3e5) cut = n * log2(n) * 4;
    if(n >= 6e5) cut = n * log2(n) * 3;

    // Compute light characters
    for(int i = 0; i < n; i++){
        int ch = A[i];
        ll freq = (ll) freq_a[ch] * (ptrs_b[ch+1] - ptrs_b[ch]);
        if(freq > cut) continue;

        for(int p = ptrs_b[ch]; p < ptrs_b[ch+1]; p++){
            int j = b_inds[p];
            ans[i-j+m-1]++;
        }
    }

    vector<int> in_a(n), in_b(m), out_fft;

    // Compute heavy characters
    for(int ch = 0; ch < sigma; ch++){
        ll freq = (ll) freq_a[ch] * (ptrs_b[ch+1] - ptrs_b[ch]);
        if(freq <= cut) break;

            for(int i = 0; i < n; i++) in_a[i] = (int) A[i] == ch;
            for(int i = 0; i < m; i++) in_b[m-1-i] = (int) B[i] == ch;
            out_fft = atcoder::convolution(in_a, in_b);
            for(int i = m-1; i < n+m-1; i++) ans[i] += out_fft[i];
    }

    for(int i = 0; i < n-m+1; i++) result[i] = m - ans[i+m-1];
}
