#include "ham_dist_proj.h"
#include "convolution.h"
#include "utils.h"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>

using namespace std;
typedef long long ll;

void ham_dist_heur(int n, int m, int sigma, double in_eps, const vector<uint32_t> &A,
                   const vector<uint32_t> &B, vector<uint32_t> &result,
                   const vector<uint32_t> &ans_ub, mt19937 &rng, bool opt_eps) {
    // Use larger eps, then multiply by (1 + in_eps) at the end
    double eps = opt_eps ? 1 - (1 - in_eps)/(1 + in_eps) : in_eps;

    int reduced_sigma = ceil(min(2/eps - F_EPS, (double) sigma));
    double c = 1; // run c * log n times
    int planned_its = ceil(c * log2(n)), act_its = 0;

    // if(num_its * reduced_sigma >= sigma){
    //     reduced_sigma = sigma;
    //     num_its = 1;
    // }

    vector<int> a_freq(sigma), b_freq(sigma);
    for(int i = 0; i < n; i++) a_freq[A[i]]++;
    for(int i = 0; i < m; i++) b_freq[B[i]]++;

    vector<int> ord(sigma);
    iota(begin(ord), end(ord), 0);

    vector<pair<ll, vector<int>>> hashes(planned_its, {-1, vector<int>(sigma)});
    for(int i = 0; i < planned_its; i++){
        shuffle(ord.begin(), ord.end(), rng);
        vector<int> bucket_a_freq(reduced_sigma), bucket_b_freq(reduced_sigma);

        for(int j = 0; j < sigma; j++){
            int old_id = ord[j];
            int new_id = j % reduced_sigma;
            hashes[i].second[old_id] = new_id;
            bucket_a_freq[new_id] += a_freq[old_id];
            bucket_b_freq[new_id] += b_freq[old_id];
        }

        ll val = 0;
        for(int j = 0; j < reduced_sigma; j++) val += (ll) bucket_a_freq[j] * bucket_b_freq[j];
        hashes[i].first = val;
    }
    sort(hashes.begin(), hashes.end());

    int sz = bit_ceil((unsigned) (n+m-1));
    vector<int> a_fft(sz), b_fft(sz), sum_res(n-m+1);

    for (int round = 0; round < planned_its; round++){
        act_its++;
        const vector<int> &mp = hashes[round].second;

        fill(sum_res.begin(), sum_res.end(), 0);

        for (int ch = 0; ch < reduced_sigma; ch++){
            for (int i = 0; i < n; i++) a_fft[i] = mp[A[i]] == ch;
            for (int i = 0; i < m; i++) b_fft[m-1-i] = mp[B[i]] != ch;
            convolution(n, m, a_fft, b_fft);
            for (int i = 0; i < n-m+1; i++) sum_res[i] += a_fft[i+m-1];
        }

        int num_bad = 0;
        for (int i = 0; i < n-m+1; i++){
            result[i] = max<int>(result[i], sum_res[i]);
            if(opt_eps) num_bad += (int) (result[i] * (1 + in_eps)) < ans_ub[i] * (1 - in_eps);
            else num_bad += result[i] < ans_ub[i] * (1 - in_eps);
        }
        if(!num_bad) break;
    }

    if(opt_eps) for (int i = 0; i < n-m+1; i++) result[i] *= 1 + in_eps;

    fprintf(stderr, "planned_its: %d, act_its: %d, reduced_sigma: %d\n",
            planned_its, act_its, reduced_sigma);
}
