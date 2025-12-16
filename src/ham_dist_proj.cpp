#include "ham_dist_proj.h"
#include "convolution.h"
#include "utils.h"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>

using namespace std;


void ham_dist_proj(int n, int m, int sigma, double in_eps, const vector<uint32_t> &A,
                   const vector<uint32_t> &B, vector<uint32_t> &result,
                   const vector<uint32_t> &ans_ub, mt19937 &rng, bool opt_eps) {
    // Use larger eps, then multiply by (1 + in_eps) at the end
    double eps = opt_eps ? 1 - (1 - in_eps)/(1 + in_eps) : in_eps;

    int reduced_sigma = ceil(min(2/eps - F_EPS, (double) sigma));
    double c = 1; // run c * log n times
    int planned_its = ceil(c * log2(n)), act_its = 0;

    // if(num_its * reduced_sigma >= sigma){
    //     reduced_sigma = sigma;
    //     num_its = 1;
    // }

    int sz = bit_ceil((unsigned) (n+m-1));
    vector<int> a_fft(sz), b_fft(sz), sum_res(n-m+1);

    // Random ordering of the alphabet
    vector<int> ord(sigma), rng_map(sigma);
    iota(begin(ord), end(ord), 0);


    for (int round = 0; round < planned_its; round++){ // run for c log n rounds
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
            if(opt_eps) num_bad += (int) (result[i] * (1 + in_eps)) < ans_ub[i] * (1 - in_eps);
            else num_bad += result[i] < ans_ub[i] * (1 - in_eps);
        }
        if(!num_bad) break;
    }

    if(opt_eps) for (int i = 0; i < n-m+1; i++) result[i] *= 1 + in_eps;

    fprintf(stderr, "planned_its: %d, act_its: %d, reduced_sigma: %d\n",
            planned_its, act_its, reduced_sigma);
}
