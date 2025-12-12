#include <cstdint>
#include <random>
#include <vector>

void HammingDistanceHeuristic_1(int n, int m, int sigma, double eps,
                                const std::vector<uint32_t> &A, const std::vector<uint32_t> &B,
                                std::vector<uint32_t> &result, const std::vector<uint32_t> &ans_ub,
                                std::mt19937& rng);
