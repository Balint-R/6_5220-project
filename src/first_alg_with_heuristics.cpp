#include "first_alg_with_heuristics.h"
#include "HAM.h"

using namespace std;

typedef function<uint32_t(size_t)> HashFunction;
typedef function<HashFunction(size_t, int, const vector<uint32_t>&, const vector<uint32_t>&, const unordered_map<int, long long>&)> HashChooser;

const int PRIME = 1e9+7;

const size_t SEED = 430298584;
mt19937 rng(SEED);

uniform_int_distribution<int> dist(1, PRIME - 1);

HashFunction choose_hash_random(size_t num_buckets) {
    int a = dist(rng);
    int b = dist(rng);
    return [=](size_t x) -> uint32_t {
        long long val = (1LL * a * x + b) % PRIME;
        return (uint32_t)(val % num_buckets);
    };
}

// choose the best h from k candidates
// that minimizes sum(bucket_mass)^2
HashFunction choose_hash_heuristic_1(size_t num_buckets, int k, const vector<uint32_t>&, const vector<uint32_t>&, const unordered_map<int, long long>& freq) {
    auto score_function = [&](HashFunction h) {
        map<uint32_t, long long> bucket_mass;
        for (const auto& [val, count] : freq) {
            uint32_t bucket = h(val);
            bucket_mass[bucket] += count;
        }
        long long score = 0;
        for (const auto& [bucket, mass] : bucket_mass) {
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

// choose the best h from k candidates
// that minimize sum(max freq over windows)^2
HashFunction choose_hash_heuristic_2 (size_t num_buckets, int k, const vector<uint32_t>& A, const vector<uint32_t>& B, const unordered_map<int, long long>& ) {
    size_t n = A.size(), m = B.size();
    if (n < m) return choose_hash_random(num_buckets);

    auto score_function = [&](HashFunction h) {
        vector<uint32_t> hA(n);
        for (size_t i = 0; i < n; i++) hA[i] = h(A[i]);

        vector<int> window_freq(num_buckets, 0);
        vector<int> max_window_freq(num_buckets, 0);

        for (size_t i = 0; i < n; i++) {
            uint32_t in = hA[i];
            window_freq[in]++;

            if (i >= m) {
                uint32_t out = hA[i - m];
                window_freq[out]--;
            }
            if (i >= m - 1) {
                max_window_freq[in] = max(max_window_freq[in], window_freq[in]);
            }
        }

        long long score = 0;
        for (size_t b = 0; b < num_buckets; b++) {
            long long mass = max_window_freq[b];
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


// main function to test various hash function choosing heuristics
void HammingDistanceHeuristic(size_t n, size_t m, size_t sigma, double eps, const vector<uint32_t> &A,
                                const vector<uint32_t> &B,
                                HashChooser choose_hash,
                                vector<uint32_t> &dist, mt19937 &rng1) {
    // parameters
    double c = 2; // run c * log n times
    size_t num_rounds = max((size_t)1, (size_t)(c * log2(n)));

    size_t num_buckets = static_cast<size_t>(2 / eps);
    int k = 10; // number of candidate hash functions

    unordered_map<int, long long> freq;
    for (size_t i = 0; i < n; i++) {
        freq[A[i]]++;
    }
    for (size_t i = 0; i < m; i++) {
        freq[B[i]]++;
    }

    for (size_t round = 0; round < num_rounds; round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = choose_hash(num_buckets, k, A, B, freq);

        // compute hash for input vectors
        vector<uint32_t> hA(n);
        vector<uint32_t> hB(m);
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);
        for (int i = 0; i < m; i++) hB[i] = h(B[i]);

        // do brute force / FFT
        vector<uint32_t> cur(n - m + 1);
        HAM((int)n, (int)m, (int)num_buckets, hA, hB, cur);
        for (size_t i = 0; i < cur.size(); i++) {
            dist[i] = max(dist[i], cur[i]);
        }
    }
}

// choose the best h from k candidates
// that minimize sum(bucket_mass)^2
void HammingDistanceHeuristic_1(size_t n, size_t m, size_t sigma, double eps, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist, mt19937& rng) {
    HashChooser choose_hash = choose_hash_heuristic_1;
    HammingDistanceHeuristic(n, m, sigma, eps, A, B, choose_hash, dist, rng);
}

// choose the best h from k candidates
// that minimize sum(max freq over windows)^2
void HammingDistanceHeuristic_2(size_t n, size_t m, size_t sigma, double eps, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist, mt19937& rng) {
    HashChooser choose_hash = choose_hash_heuristic_2;
    HammingDistanceHeuristic(n, m, sigma, eps, A, B, choose_hash, dist, rng);
}

void HammingDistanceBase(size_t n, size_t m, size_t sigma, double eps, const vector<uint32_t> &A,
                         const vector<uint32_t> &B, vector<uint32_t> &dist, mt19937 &rng1) {
    // parameters
    double c = 2; // run c * log n times

    size_t num_buckets = static_cast<size_t>(2 / eps);

    for (size_t round = 0; round < (size_t) (c * log2(n)); round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = choose_hash_random(num_buckets);

        // compute hash for input vectors
        vector<uint32_t> hA(n);
        vector<uint32_t> hB(m);
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);
        for (int i = 0; i < m; i++) hB[i] = h(B[i]);

        // do brute force / FFT
        vector<uint32_t> cur(n - m + 1);
        HAM_fft((int)n, (int)m, (int)num_buckets, hA, hB, cur);
        for (size_t i = 0; i < cur.size(); i++) {
            dist[i] = max(dist[i], cur[i]);
        }
    }
}
