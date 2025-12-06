#include <vector>
#include <cstdint>
#include <random>

void HammingDistanceProj(int n, int m, int sigma, double eps,
    const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist,
    std::mt19937& rng);
