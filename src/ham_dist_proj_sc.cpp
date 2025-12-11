#include "ham_dist_proj.h"
#include "convolution.h"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>

using namespace std;


void ham_dist_proj_sc(int n, int m, int sigma, double in_eps,
                   const vector<uint32_t> &A, const vector<uint32_t> &B,
                   vector<uint32_t> &result, mt19937 &rng) {
    // Use larger eps, then multiply by (1 + in_eps) at the end
    double eps = 1 - (1 - in_eps)/(1 + in_eps);
    int reduced_sigma = ceil(min(2 / eps, (double) sigma));
    double c = 0.5; // run c * log n times
    int planned_its = c * ceil(log2(n));

    if(ceil(2/eps) >= sigma){
        reduced_sigma = sigma;
        planned_its = 1;
    }

    int sz = bit_ceil((unsigned) (n+m-1));
    vector<int> a_fft(sz), b_fft(sz), sum_res(n-m+1);

    // Random ordering of the alphabet
    vector<int> ord(sigma), rng_map(sigma);
    iota(begin(ord), end(ord), 0);

    int rem_its = planned_its, act_its = 0;

    while(rem_its--){ // run for c log n rounds
        act_its++;
        shuffle(begin(ord), end(ord), rng);
        for (int i = 0; i < sigma; i++) rng_map[ord[i]] = i % reduced_sigma;

        fill(sum_res.begin(), sum_res.end(), 0);
        // Corollary 4: for each character in reduced alphabet, assign corresponding binary value
        for (int ch = 0; ch < reduced_sigma; ch++){
            for (int i = 0; i < n; i++) a_fft[i] = rng_map[A[i]] == ch;
            for (int i = 0; i < m; i++) b_fft[m-1-i] = rng_map[B[i]] != ch;
            convolution(n, m, a_fft, b_fft);
            for (int i = 0; i < n-m+1; i++) sum_res[i] += a_fft[i+m-1];
        }

        int num_bad = 0;
        for (int i = 0; i < n-m+1; i++){
            result[i] = max<int>(result[i], sum_res[i]);
            num_bad += result[i] < m*(1 - eps);
        }

        if(!num_bad) break;
        rem_its = min<int>(rem_its, ceil(log2(num_bad) + log2(n)));
    }

    for (int i = 0; i < n-m+1; i++) result[i] *= 1 + in_eps;

    fprintf(stderr, "planned_its: %d, act_its: %d, reduced_sigma: %d\n",
            planned_its, act_its, reduced_sigma);
}
