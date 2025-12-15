#include "first_alg_with_heuristics.h"
#include "convolution.h"
#include "utils.h"
#include <bit>
#include <functional>
#include <algorithm>

using namespace std;

typedef function<uint32_t(int)> HashFunction;
typedef function<HashFunction(int, int, const vector<uint32_t>&, const vector<uint32_t>&, vector<uint32_t>&)> HashChooser;

const int PRIME = 1e9+7;

const int SEED = 430298584;
mt19937 rng_global(SEED);

uniform_int_distribution<int> uni_dist(1, PRIME - 1);

inline HashFunction choose_hash_random(int num_buckets) {
    int a = uni_dist(rng_global);
    int b = uni_dist(rng_global);
    return [=](int x) -> uint32_t {
        long long val = (1LL * a * x + b) % PRIME;
        return (uint32_t)(val % num_buckets);
    };
}

// choose the best h from k candidates
// that minimizes sum(bucket_mass)^2
HashFunction choose_hash_heuristic_1(int num_buckets, int k, const vector<uint32_t>&, const vector<uint32_t>&, vector<uint32_t>& freq) {
    vector<long long> bucket_mass(num_buckets);

    auto score_function = [&](HashFunction h) {
        fill(bucket_mass.begin(), bucket_mass.end(), 0);
        for (int val = 0; val < (int) freq.size(); val++) {
            uint32_t bucket = h((int)val);
            bucket_mass[bucket] += freq[val];
        }
        long long score = 0;
        for (int b = 0; b < num_buckets; b++) {
            long long mass = bucket_mass[b];
            score += mass * mass;
        }
        return score;
    };


    auto best_h = choose_hash_random(num_buckets);
    long long min_score = score_function(best_h);
    for (int i = 1; i < k; i++) {
        auto cand = choose_hash_random(num_buckets);
        long long score = score_function(cand);
        if (score < min_score) {
            min_score = score;
            best_h = cand;
        }
    }
    return best_h;
}

void HAM_fft(int n, int m, int sigma, const vector<uint32_t> &A,
             const vector<uint32_t> &B, vector<uint32_t> &result) {
    int sz = bit_ceil((unsigned) (n+m-1));
    vector<int> a_fft(sz), b_fft(sz);

    // For each character in the FULL alphabet, assign corresponding binary value
    for (int ch = 0; ch < sigma; ch++) {
        for (int i = 0; i < n; i++) a_fft[i] = (int) A[i] == ch;
        for (int i = 0; i < m; i++) b_fft[m-1-i] = (int) B[i] != ch;
        convolution(n, m, a_fft, b_fft);
        for (int i = 0; i < n-m+1; i++) result[i] += a_fft[i+m-1];
    }
}

// main function to test various hash function choosing heuristics
void HammingDistanceHeuristic(int n, int m, int sigma, double in_eps,
                              const vector<uint32_t> &A,
                              const vector<uint32_t> &B,
                              HashChooser choose_hash,
                              vector<uint32_t> &result,
                              const vector<uint32_t> &ans_ub,
                              mt19937 &rng1) {

    // Use larger eps, then multiply by (1 + in_eps) at the end
    double eps = 1 - (1 - in_eps)/(1 + in_eps);
    int reduced_sigma = ceil(min(2/eps - F_EPS, (double) sigma));
    double c = 1.0;

    int planned_its = ceil(c * log2(n)); // R = c log n
    int act_its = 0;

    // Global symbol frequencies in T and P
    vector<uint32_t> freqA(sigma, 0), freqB(sigma, 0);
    for (int i = 0; i < n; i++) freqA[A[i]]++;
    for (int i = 0; i < m; i++) freqB[B[i]]++;

    vector<uint32_t> hA(n);
    vector<uint32_t> hB(m);

    int S = planned_its; // number of hash functions to sample
    double S_frac = 0.5;

    vector<pair<long long, HashFunction>> candidates;
    candidates.reserve(S);

    auto score_AB = [&](const HashFunction &h) -> long long {
        vector<long long> bucketA(reduced_sigma, 0), bucketB(reduced_sigma, 0);

        for (int s = 0; s < sigma; s++) {
            uint32_t b = h(s);
            if (b >= (uint32_t)reduced_sigma) b %= reduced_sigma;
            bucketA[b] += freqA[s];
            bucketB[b] += freqB[s];
        }

        long long score = 0;
        for (int b = 0; b < reduced_sigma; b++) {
            score += bucketA[b] * bucketB[b];
        }
        return score;
    };

    // Generate S random candidate hash functions and score them
    for (int i = 0; i < S; i++) {
        HashFunction h = choose_hash_random(reduced_sigma);
        long long s = score_AB(h);
        candidates.emplace_back(s, std::move(h));
    }

    // sort by score
    sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    int rounds = ceil(S_frac * candidates.size());

    for (int round = 0; round < rounds; round++) {
        act_its++;

        const HashFunction &h = candidates[round].second;

        // compute hash for input vectors
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);
        for (int i = 0; i < m; i++) hB[i] = h(B[i]);

        // do FFT
        int num_bad = 0;
        vector<uint32_t> cur(n-m+1);
        HAM_fft(n, m, reduced_sigma, hA, hB, cur);
        for (int i = 0; i < n-m+1; i++) {
            result[i] = max(result[i], cur[i]);
            num_bad += (int) (result[i] * (1 + in_eps)) < ans_ub[i] * (1 - in_eps);
        }
        if(!num_bad) break;
    }

    for (int i = 0; i < n-m+1; i++) result[i] = result[i] * (1 + in_eps);

    fprintf(stderr, "planned_its: %d, act_its: %d, reduced_sigma: %d\n",
            rounds, act_its, reduced_sigma);
}


// choose the best h from k candidates
// that minimize sum(bucket_mass)^2
void HammingDistanceHeuristic_1(int n, int m, int sigma, double eps,
                                const vector<uint32_t> &A, const vector<uint32_t> &B,
                                vector<uint32_t> &result, const vector<uint32_t> &ans_ub,
                                mt19937 &rng) {
    HashChooser choose_hash = choose_hash_heuristic_1;
    HammingDistanceHeuristic(n, m, sigma, eps, A, B, choose_hash, result, ans_ub, rng);
}
