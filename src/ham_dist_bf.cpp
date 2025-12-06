#include "ham_dist_bf.h"

void ham_dist_bf(size_t n, size_t m, const std::vector<uint32_t> &A,
                       const std::vector<uint32_t> &B, std::vector<uint32_t> &dist){
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
