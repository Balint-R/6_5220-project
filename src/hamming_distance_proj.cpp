#include "hamming_distance_proj.h"
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

void HammingDistanceProj(size_t n, size_t m, size_t sigma, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, 
                        std::vector<uint32_t>& dist, std::mt19937& rng) {
    // parameters
    double eps = 0.01;
    double c = 2; // run c * log n times

    for (size_t round = 0; round < (size_t) (c * std::log2(n)); round ++){ // run for c log n rounds
        // pick 2-universal hash function
        uint32_t p = ModPrimeHash.p;
        ModPrimeHash h(std::uniform_int_distribution(1, p-1)(rng), std::uniform_int_distribution(0, p-1)(rng));

        // hash alphabet vectors into binary vectors
        // run fft algorithm
        // update dist array

    }
}
