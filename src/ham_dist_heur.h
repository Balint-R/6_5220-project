#include <vector>
#include <cstdint>
#include <random>

void ham_dist_heur(int n, int m, int sigma, double eps,
    const std::vector<uint32_t> &A, const std::vector<uint32_t> &B,
    std::vector<uint32_t> &result, const std::vector<uint32_t> &ans_ub, std::mt19937 &rng);
