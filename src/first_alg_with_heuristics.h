#include <cmath>
#include <vector>
#include <cstdint>
#include <random>
#include <functional>
#include <map>
#include <cassert>
#include <algorithm>

std::function<int(size_t)> generate_hash(size_t num_buckets);

std::function<int(size_t)> choose_hash_heuristic_1(size_t num_buckets, int k, std::map<int, int>& freq);

void HammingDistanceBase(size_t n, size_t m, size_t sigma, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);

void HammingDistanceHeuristic_1(size_t n, size_t m, size_t sigma, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);
