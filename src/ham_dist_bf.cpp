#include "ham_dist_bf.h"
using namespace std;

void ham_dist_bf(int n, int m, int sigma, const vector<uint32_t> &A,
                       const vector<uint32_t> &B, vector<uint32_t> &dist){
    for (int i = 0; i < n - m + 1; i++) {
        uint32_t cur_dist = 0;
        for (int j = 0; j < m; j++) cur_dist += A[i+j] != B[j];
        dist[i] = cur_dist;
    }
}
