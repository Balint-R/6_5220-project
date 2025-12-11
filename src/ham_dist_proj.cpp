#include "ham_dist_proj.h"
#include "atcoder/convolution.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace std;


void ham_dist_proj(int n, int m, int sigma, double in_eps,
                   const vector<uint32_t> &A, const vector<uint32_t> &B,
                   vector<uint32_t> &result, mt19937 &rng) {
    // Use larger eps, then multiply by (1 + in_eps) at the end
    double eps = 1 - (1 - in_eps)/(1 + in_eps);
    
    int reduced_sigma = ceil(min(2 / eps, (double) sigma));
    double c = 0.5; // run c * log n times
    int num_its = c * ceil(log2(n));

    // if(num_its * reduced_sigma >= sigma){
    //     reduced_sigma = sigma;
    //     num_its = 1;
    // }

    vector<uint32_t> binA(n), binB(m), binOut(n+m-1), aggBinOut(n-m+1); // scratch arrays to run FFT on

    // Random ordering of the alphabet
    vector<int> ord(sigma), rng_map(sigma);
    iota(begin(ord), end(ord), 0);

    fprintf(stderr, "num_its: %d, reduced_sigma: %d\n", num_its, reduced_sigma);

    for (int round = 0; round < num_its; round++){ // run for c log n rounds
        shuffle(begin(ord), end(ord), rng);
        for (int i = 0; i < sigma; i++) rng_map[ord[i]] = i % reduced_sigma;

        fill(begin(aggBinOut), end(aggBinOut), 0);

        // Corollary 4: for each character in reduced alphabet, assign corresponding binary value
        for (int ch = 0; ch < reduced_sigma; ch++){
            for (int i = 0; i < n; i++) binA[i] = rng_map[A[i]] == ch;
            for (int i = 0; i < m; i++) binB[m-1-i] = rng_map[B[i]] != ch;
            binOut = atcoder::convolution(binA, binB);
            for (int i = 0; i < n-m+1; i++) aggBinOut[i] += binOut[i+m-1];
        }

        for (int i = 0; i < n-m+1; i++) result[i] = max(result[i], aggBinOut[i]);
    }

    for (int i = 0; i < n-m+1; i++) result[i] *= 1 + in_eps;
}
