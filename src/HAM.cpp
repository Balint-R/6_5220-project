#include "HAM.h"
#include "convolution.h"
#include <bit>

using namespace std;

// Compute exact Hamming distance using O(nm) brute force
void BRUTE(int n, int m, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist) {
    // do brute force
    for (int i = 0; i < n - m + 1; i++) {
        int cur_dist = 0;
        for (int j = 0; j < m; j++) cur_dist += A[i+j] != B[j];
        dist[i] = cur_dist;
    }
}

// Compute exact Hamming distance using FFT
void HAM_fft(int n, int m, int sigma,
                   const vector<uint32_t> &A, const vector<uint32_t> &B,
                   vector<uint32_t> &result) {

    int sz = bit_ceil((unsigned) (n+m-1));
    vector<int> a_fft(sz), b_fft(sz);

    // For each character in the FULL alphabet, assign corresponding binary value
    for (int ch = 0; ch < sigma; ch++) {
        for (int i = 0; i < n; i++) a_fft[i] = A[i] == ch;
        for (int i = 0; i < m; i++) b_fft[m-1-i] = B[i] != ch;
        convolution(n, m, a_fft, b_fft);
        for (int i = 0; i < n-m+1; i++) result[i] += a_fft[i+m-1];
    }
}

// Compute the alphabet size (number of distinct characters)
inline int alphabet_size(const vector<uint32_t>& A, const vector<uint32_t>& B) {
    set<uint32_t> unique_chars(A.begin(), A.end());
    unique_chars.insert(B.begin(), B.end());
    return unique_chars.size();
}

// HAM is the inner fucntion that computes exact Hamming distance in the paper
void HAM(int n, int m, int sigma, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist) {
    // return ham_dist_sqrt(n, m, sigma, 0.1, A, B, dist);
    return HAM_fft(n, m, sigma, A, B, dist);

    // double cost_bf = 1.0 * n * m;
    // double fft_const = 10.0; // guess constant for FFT cost

    // double fft_one = fft_const * (n + m) * log2(n + m);
    // double cost_fft = fft_one * sigma;
    // if (cost_bf <= cost_fft) {
    //     // cout << "Using BRUTE" << endl;
    //     BRUTE(n, m, A, B, dist);
    // } else {
    //     // cout << "Using HAM_fft" << endl;
    //     HAM_fft(n, m, sigma, A, B, dist);
    // }
}
