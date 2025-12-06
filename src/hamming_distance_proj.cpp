#include "hamming_distance_proj.h"
#include "atcoder/convolution.hpp"
#include <algorithm>
#include <cmath>


struct ModPrimeHash {
    uint32_t a;
    uint32_t b;
    static constexpr uint32_t p = 4294967291u; // largest 32-bit prime

    ModPrimeHash(uint32_t a_, uint32_t b_) : a(a_), b(b_) {
        if (a == 0) a = 1; // ensure a != 0 for universality
    }

    inline uint32_t operator()(uint32_t x) const {
        uint64_t ax = uint64_t(a) * uint64_t(x);   
        uint64_t axb = ax + b;
        uint64_t mod = axb % p;                 
        return uint32_t(mod);
    }
};

void HammingDistanceProj(size_t n, size_t m, size_t sigma, double eps,
                         const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, 
                         std::vector<uint32_t>& dist, std::mt19937& rng) {
    size_t reduced_sigma = std::ceil(std::min(2 / eps, (double) sigma));
    size_t c = 5; // run c * log n times
    size_t num_its = c * std::ceil(std::log2(n));
    std::vector<uint32_t> binA(n), binB(m), binOut(n+m-1), aggBinOut(n-m+1); // scratch arrays to run FFT on

    for (size_t round = 0; round < num_its; round++){ // run for c log n rounds
        // pick 2-universal hash function
        uint32_t p = ModPrimeHash::p;
        ModPrimeHash h(std::uniform_int_distribution(1u, p-1)(rng), std::uniform_int_distribution(0u, p-1)(rng));
        aggBinOut = std::vector<uint32_t>(n-m+1, 0);

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
            dist[i] = std::max(dist[i], aggBinOut[i]);
        }
    }
}
