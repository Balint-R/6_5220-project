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
#include <set>

using namespace std;

void BRUTE(size_t n, size_t m, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist);

void HAM_fft(int n, int m, int sigma, const vector<uint32_t> &A, const vector<uint32_t> &B, vector<uint32_t> &result);

void HAM(size_t n, size_t m, size_t sigma, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist);
