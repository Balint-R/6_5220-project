#include "ham_dist_sqrt.h"
#include "atcoder/convolution.hpp"
#include <cmath>

using namespace std;

typedef long long ll;

void ham_dist_sqrt(int n, int m, int sigma, double eps,
                   const vector<uint32_t> &A, const vector<uint32_t> &B, 
                   vector<uint32_t> &result) {

    // dbgArr(result, SZ(result));

    vector<int> freq_a(sigma), freq_b(sigma);
    // Linked lists
    vector<int> head_a(sigma, -1), head_b(sigma, -1);
    vector<int> nxt_a(n, -1), nxt_b(m, -1);

    for(int i = n-1; i >= 0; i--){
        nxt_a[i] = head_a[A[i]];
        head_a[A[i]] = i;
        freq_a[A[i]]++;
    }

    for(int i = m-1; i >= 0; i--){
        nxt_b[i] = head_b[B[i]];
        head_b[B[i]] = i;
        freq_b[B[i]]++;
    }

    int cut = n * log(n) * 20;

    vector<int> a_ls;
    a_ls.reserve(n);

    // Number of matches per position, padded at start so no index out of bounds
    vector<int> big_result(n+m-1);

    // Vectors to pass to FFT
    vector<int> in_a(n), in_b(m), out_fft(n+m-1);

    // dbg(cut);

    for(int s = 0; s < sigma; s++){
        // cerr << freq_a[s] << ' ' << freq_b[s] << ' ' << (ll) freq_a[s] * freq_b[s] << endl;

        if((ll) freq_a[s] * freq_b[s] < cut){
            for(int i = head_a[s]; i != -1; i = nxt_a[i]) a_ls.push_back(i);
            for(int j = head_b[s]; j != -1; j = nxt_b[j]){
                for(int i : a_ls) big_result[i-j+m-1]++;
            }
            a_ls.clear();
        }
        else {
            for(int i = 0; i < n; i++) in_a[i] = (int) A[i] == s;
            for(int i = 0; i < m; i++) in_b[m-1-i] = (int) B[i] == s;
            out_fft = atcoder::convolution(in_a, in_b);
            for(int i = m-1; i < n+m-1; i++) big_result[i] += out_fft[i];
        }
    }

    for(int i = 0; i < n-m+1; i++) result[i] = m - big_result[i+m-1];

    // dbgArr(result, SZ(result));
}
