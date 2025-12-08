#include "first_alg_with_heuristics.h"
#include "HAM.h"

using namespace std;

typedef function<uint32_t(int)> HashFunction;
typedef function<HashFunction(int, int, const vector<uint32_t>&, const vector<uint32_t>&, const vector<uint32_t>&)> HashChooser;

const int PRIME = 1e9+7;

const int SEED = 430298584;
mt19937 rng(SEED);

uniform_int_distribution<int> dist(1, PRIME - 1);

HashFunction choose_hash_random(int num_buckets) {
    int a = dist(rng);
    int b = dist(rng);
    return [=](int x) -> uint32_t {
        long long val = (1LL * a * x + b) % PRIME;
        return (uint32_t)(val % num_buckets);
    };
}

// choose the best h from k candidates
// that minimizes sum(bucket_mass)^2
HashFunction choose_hash_heuristic_1(int num_buckets, int k, const vector<uint32_t>&, const vector<uint32_t>&, const vector<uint32_t>& freq) {
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

// choose the best h from k candidates
// that minimize sum(max freq over windows)^2
HashFunction choose_hash_heuristic_2 (int num_buckets, int k, const vector<uint32_t>& A, const vector<uint32_t>& B, const vector<uint32_t>& ) {
    int n = A.size(), m = B.size();
    if (n < m) return choose_hash_random(num_buckets);

    vector<uint32_t> hA(n), window_freq(num_buckets), max_window_freq(num_buckets);

    auto score_function = [&](HashFunction h) {
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);

        fill(window_freq.begin(), window_freq.end(), 0);
        fill(max_window_freq.begin(), max_window_freq.end(), 0);

        for (int i = 0; i < n; i++) {
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
        for (int b = 0; b < num_buckets; b++) {
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

// choose c = O(1/eps) but capped to [MIN, MAX]
double choose_c(double eps) {
    double MIN = 0.0, MAX = 2.0;
    double c = 0.00001 / eps;
    if (c < MIN) return MIN;
    if (c > MAX) return MAX;
    return c;
}

// main function to test various hash function choosing heuristics
void HammingDistanceHeuristic(int n, int m, int sigma, double eps, const vector<uint32_t> &A,
                                const vector<uint32_t> &B,
                                HashChooser choose_hash,
                                vector<uint32_t> &dist, mt19937 &rng1) {
    // parameters
    double c = choose_c(eps); // run c * log n times
    int num_rounds = max((int)1, (int)(c * log2(n)));
    // printf("c = %.4f, num_rounds = %d\n", c, (int)num_rounds);

    int num_buckets = min(int(2 / eps), (int)sigma);
    int k = 4; // number of candidate hash functions

    vector<uint32_t> freq(sigma, 0);
    for (int i = 0; i < n; i++) {
        freq[A[i]]++;
    }
    for (int i = 0; i < m; i++) {
        freq[B[i]]++;
    }

    vector<uint32_t> hA(n);
    vector<uint32_t> hB(m);

    for (int round = 0; round < num_rounds; round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = choose_hash(num_buckets, k, A, B, freq);

        // compute hash for input vectors
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);
        for (int i = 0; i < m; i++) hB[i] = h(B[i]);

        // do brute force / FFT
        vector<uint32_t> cur(n - m + 1);
        HAM(n, m, num_buckets, hA, hB, cur);
        for (int i = 0; i < (int) cur.size(); i++) {
            dist[i] = max(dist[i], cur[i]);
        }
    }
}

// choose the best h from k candidates
// that minimize sum(bucket_mass)^2
void HammingDistanceHeuristic_1(int n, int m, int sigma, double eps, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist, mt19937& rng) {
    HashChooser choose_hash = choose_hash_heuristic_1;
    HammingDistanceHeuristic(n, m, sigma, eps, A, B, choose_hash, dist, rng);
}

// choose the best h from k candidates
// that minimize sum(max freq over windows)^2
void HammingDistanceHeuristic_2(int n, int m, int sigma, double eps, const vector<uint32_t>& A, const vector<uint32_t>& B, vector<uint32_t>& dist, mt19937& rng) {
    HashChooser choose_hash = choose_hash_heuristic_2;
    HammingDistanceHeuristic(n, m, sigma, eps, A, B, choose_hash, dist, rng);
}

void HammingDistanceBase(int n, int m, int sigma, double eps, const vector<uint32_t> &A,
                         const vector<uint32_t> &B, vector<uint32_t> &dist, mt19937 &rng1) {
    // parameters
    double c = 2; // run c * log n times

    int num_buckets = static_cast<int>(2 / eps);

    for (int round = 0; round < (int) (c * log2(n)); round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = choose_hash_random(num_buckets);

        // compute hash for input vectors
        vector<uint32_t> hA(n);
        vector<uint32_t> hB(m);
        for (int i = 0; i < n; i++) hA[i] = h(A[i]);
        for (int i = 0; i < m; i++) hB[i] = h(B[i]);

        // do brute force / FFT
        vector<uint32_t> cur(n - m + 1);
        HAM(n, m, num_buckets, hA, hB, cur);
        for (int i = 0; i < (int) cur.size(); i++) {
            dist[i] = max(dist[i], cur[i]);
        }
    }
}
