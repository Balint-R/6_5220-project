#include <vector>
#include <cstdint>

void HammingDistanceProj(size_t n, size_t m, size_t sigma, 
    const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist,
    std::mt19937& rng);
