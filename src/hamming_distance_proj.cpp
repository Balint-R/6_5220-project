#include "hamming_distance_proj.h"
#include "atcoder/convolution.hpp"
#include <algorithm>
#include <cmath>

using namespace std;

struct ModPrimeHash {
    uint32_t a;
    uint32_t b;
    static constexpr uint32_t P = 4294967291u; // largest 32-bit prime

    ModPrimeHash(uint32_t a_, uint32_t b_) : a(a_), b(b_) {
        if (a == 0) a = 1; // ensure a != 0 for universality
    }

    inline uint32_t operator()(uint32_t x) const {
        uint64_t ax = uint64_t(a) * uint64_t(x);   
        uint64_t axb = ax + b;
        uint64_t mod = axb % P;                 
        return uint32_t(mod);
    }
};

void HammingDistanceProj(size_t n, size_t m, size_t sigma, double eps,
                         const vector<uint32_t> &A, const vector<uint32_t> &B, 
                         vector<uint32_t> &result, mt19937 &rng) {

    auto randUInt = [&](uint32_t a, uint32_t b){
        return uniform_int_distribution(a, b)(rng);
    };

    size_t reduced_sigma = ceil(min(2 / eps, (double) sigma));
    size_t c = 2; // run c * log n times
    size_t num_its = c * ceil(log2(n));

    if(num_its * reduced_sigma >= sigma){
        reduced_sigma = sigma;
        num_its = 1;
    }

    vector<uint32_t> binA(n), binB(m), binOut(n+m-1), aggBinOut(n-m+1); // scratch arrays to run FFT on

    for (size_t round = 0; round < num_its; round++){ // run for c log n rounds
        // pick 2-universal hash function
        uint32_t p = ModPrimeHash::P;
        ModPrimeHash h(randUInt(1, p-1), randUInt(0, p-1));
        aggBinOut = vector<uint32_t>(n-m+1, 0);

        // Corollary 4: for each character in reduced alphabet, assign corresponding binary value
        for (size_t ch = 0; ch < reduced_sigma; ch++){
            for (size_t i = 0; i < n; i++){
                binA[i] = ((h(A[i]) % reduced_sigma) == ch);
            }
            for (size_t i = 0; i < m; i++){
                binB[m-1-i] = ((h(B[i]) % reduced_sigma) != ch);
            }

            binOut = atcoder::convolution(binA, binB);

            for (size_t i = 0; i<n-m+1; i++){
                aggBinOut[i] += binOut[i+m-1];
            }
        }
        // update dist array
        for (size_t i = 0; i < n-m+1; i++){
            result[i] = max(result[i], aggBinOut[i]);
        }
    }
}
