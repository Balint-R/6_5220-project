#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>
#include <random>
#include <functional>
#include <map>
#include <unordered_map>
#include <cassert>
#include <algorithm>

void HammingDistanceBase(int n, int m, int sigma, double eps, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);

void HammingDistanceHeuristic_1(int n, int m, int sigma, double eps, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);

void HammingDistanceHeuristic_2(size_t n, size_t m, size_t sigma, double eps, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);

void HammingDistanceHeuristic_3(size_t n, size_t m, size_t sigma, double eps, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B, std::vector<uint32_t>& dist, std::mt19937& rng);
