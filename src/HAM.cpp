#include "HAM.h"
#include "atcoder/convolution.hpp"

using namespace std;

// Compute exact Hamming distance using O(nm) brute force
void BRUTE(size_t n, size_t m, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist) {
    // do brute force
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

// Compute exact Hamming distance using FFT
void HAM_fft(int n, int m, int sigma,
                   const vector<uint32_t> &A, const vector<uint32_t> &B,
                   vector<uint32_t> &result) {

    vector<long long> binA(n), binB(m), binOut; // scratch arrays to run FFT on
    vector<uint32_t> aggBinOut(n - m + 1, 0);

    // For each character in the FULL alphabet, assign corresponding binary value
    for (int ch = 0; ch < sigma; ch++) {
        for (int i = 0; i < n; i++) {
            binA[i] = (A[i] == (uint32_t)ch) ? 1 : 0;
        }
        for (int j = 0; j < m; j++) {
            binB[m - 1 - j] = (B[j] != (uint32_t)ch) ? 1 : 0;
        }

        // for each shift i, convolution counts the number of positions j with:
        // A[i + j] == ch and B[j] != ch
        binOut = atcoder::convolution(binA, binB);

        for (int i = 0; i < n - m + 1; i++) {
            aggBinOut[i] += (uint32_t)binOut[i + m - 1];
        }
    }

    for (int i = 0; i < n - m + 1; i++) {
        result[i] = aggBinOut[i];
    }
}

// Compute the alphabet size (number of distinct characters)
int alphabet_size(const vector<uint32_t>& A, const vector<uint32_t>& B) {
    set<uint32_t> unique_chars(A.begin(), A.end());
    unique_chars.insert(B.begin(), B.end());
    return unique_chars.size();
}

// HAM is the inner fucntion that computes exact Hamming distance in the paper
void HAM(size_t n, size_t m, size_t sigma, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist) {
    double cost_bf = 1.0 * n * m;
    double fft_const = 10.0; // guess constant for FFT cost
    double cost_fft = fft_const * (n + m) * log2(n + m) * sigma;
    if (cost_bf < cost_fft) {
        // cout << "Using brute force for sigma = " << sigma << "\n";
        BRUTE(n, m, A, B, dist);
        return;
    } else {
        // cout << "Using FFT for sigma = " << sigma << "\n";
        HAM_fft(n, m, sigma, A, B, dist);
    }
}
