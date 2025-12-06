#include <vector>
#include <cstdint>
#include <random>

void ham_dist_sqrt(int n, int m, int sigma, double eps,
    const std::vector<uint32_t> &A, const std::vector<uint32_t> &B,
    std::vector<uint32_t> &result, std::mt19937 &rng);
