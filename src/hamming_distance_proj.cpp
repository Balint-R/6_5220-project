#include "hamming_distance_proj.h"
#include <cmath>
#include <random>

void HammingDistanceProj(size_t n, size_t m, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, 
                        std::vector<uint32_t>& dist, size_t sigma, std::mt19937& rng) {
    // pick 2-universal hash function

    // parameters
    double eps = 0.01;
    double c = 2; // run c * log n times

    std::uniform_int_distribution(a, b)(rng)


    for (size_t round = 0; round < (size_t) (c * std::log2(n)); round ++){ // run for c log n rounds
        

    }
    
    
    for (uint32_t i = 0; i < n - m + 1; i++) {
        uint32_t cur_dist = 0;
        for (uint32_t j = 0; j < m; j++) {
            if (A[i + j] != B[j]) {
                cur_dist++;
            }
        }
        dist[i] = cur_dist;
    }
}
