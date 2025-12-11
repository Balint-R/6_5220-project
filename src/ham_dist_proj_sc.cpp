#include "ham_dist_proj.h"
#include "atcoder/convolution.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace std;


void ham_dist_proj_sc(int n, int m, int sigma, double eps,
                   const vector<uint32_t> &A, const vector<uint32_t> &B,
                   vector<uint32_t> &result, mt19937 &rng) {

    int reduced_sigma = ceil(min(2 / eps, (double) sigma));
    double c = 0.5; // run c * log n times
    int planned_its = c * ceil(log2(n));

    if(ceil(2/eps) >= sigma){
        reduced_sigma = sigma;
        planned_its = 1;
    }

    vector<uint32_t> binA(n), binB(m), binOut(n+m-1), aggBinOut(n-m+1); // scratch arrays to run FFT on

    // Random ordering of the alphabet
    vector<int> ord(sigma), rng_map(sigma);
    iota(begin(ord), end(ord), 0);

    int rem_its = planned_its, act_its = 0;

    while(rem_its--){ // run for c log n rounds
        act_its++;
        shuffle(begin(ord), end(ord), rng);
        for(int i = 0; i < sigma; i++) rng_map[ord[i]] = i % reduced_sigma;

        fill(begin(aggBinOut), end(aggBinOut), 0);

        // Corollary 4: for each character in reduced alphabet, assign corresponding binary value
        for (int ch = 0; ch < reduced_sigma; ch++){
            for (int i = 0; i < n; i++) binA[i] = rng_map[A[i]] == ch;
            for (int i = 0; i < m; i++) binB[m-1-i] = rng_map[B[i]] != ch;

            binOut = atcoder::convolution(binA, binB);

            for (int i = 0; i < n-m+1; i++) aggBinOut[i] += binOut[i+m-1];
        }

        int num_bad = 0;
        for (int i = 0; i < n-m+1; i++){
            result[i] = max(result[i], aggBinOut[i]);
            num_bad += result[i] < m*(1 - eps);
        }

        if(!num_bad) break;
        rem_its = min<int>(rem_its, ceil(log2(num_bad) + log2(n)));
    }

    fprintf(stderr, "planned_its: %d, act_its: %d, reduced_sigma: %d\n",
            planned_its, act_its, reduced_sigma);
}
