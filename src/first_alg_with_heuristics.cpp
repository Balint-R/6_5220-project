#include "first_alg_with_heuristics.h"

// using namespace std;

const int PRIME = 1e9+7;

std::function<int(size_t)> generate_hash(size_t num_buckets) {
    int a = rand() % PRIME;
    int b = rand() % PRIME;
    return [&](size_t x) {
        return ((a * x + b) % PRIME) % num_buckets;
    };
}

// For now we're using brute force, will replace with FFT later
std::vector<int> HAM(size_t n, size_t m, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B) {
    std::vector<int> dist(n - m + 1);
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
    return dist;
}

// choose the best h from k candidates
// that minimizes sum(bucket mass)^2
std::function<int(size_t)> choose_hash_heuristic_1(size_t num_buckets, int k, std::map<int, int>& freq) {
    // std::vector<std::pair<int, int>> candidates;

    auto score_function = [&](std::function<int(size_t)> h) {
        // std::vector<long long> bucket_mass(num_buckets, 0);
        std::map<int, long long> bucket_mass;
        for (const auto& [val, count] : freq) {
            int bucket = h(val);
            bucket_mass[bucket] += count;
        }
        long long score = 0;
        for (const auto& [symbol, mass] : bucket_mass) {
            score += mass * mass;
        }
        return score;
    };

    auto best_h = generate_hash(num_buckets);
    long long min_score = score_function(best_h);
    for (int i = 1; i < k; i++) {
        auto cand = generate_hash(num_buckets);
        long long score = score_function(cand);
        if (score < min_score) {
            min_score = score;
            best_h = cand;
        }
    }
    return best_h;
}

void HammingDistanceHeuristic_1(size_t n, size_t m, size_t sigma, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B,
                        std::vector<uint32_t>& dist, std::mt19937& rng) {
    // parameters
    double eps = 0.01;
    double c = 2; // run c * log n times

    size_t num_buckets = static_cast<size_t>(2 / eps);
    int k = 5; // number of candidate hash functions

    std::map<int, int> freq;
    for (size_t i = 0; i < n; i++) {
        freq[A[i]]++;
    }
    for (size_t i = 0; i < m; i++) {
        freq[B[i]]++;
    }

    for (size_t round = 0; round < (size_t) (c * std::log2(n)); round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = choose_hash_heuristic_1(num_buckets, k, freq);

        // compute hash for input vectors
        std::vector<uint32_t> hA = A;
        std::vector<uint32_t> hB = B;
        for (auto& val : hA) {
            val = h(val);
        }
        for (auto& val : hB) {
            val = h(val);
        }

        // do brute force
        auto cur_res = HAM(n, m, hA, hB);
        assert(cur_res.size() == dist.size());
        for (size_t i = 0; i < cur_res.size(); i++) {
            dist[i] = std::max(dist[i], (uint32_t)cur_res[i]);
        }
    }

}


void HammingDistanceBase(size_t n, size_t m, size_t sigma, const std::vector<uint32_t>& A, const std::vector<uint32_t>& B,
                        std::vector<uint32_t>& dist, std::mt19937& rng) {
    // parameters
    double eps = 0.01;
    double c = 2; // run c * log n times

    size_t num_buckets = static_cast<size_t>(2 / eps);

    for (size_t round = 0; round < (size_t) (c * std::log2(n)); round++){ // run for c log n rounds
        // pick 2-universal hash function
        auto h = generate_hash(num_buckets);

        // compute hash for input vectors
        std::vector<uint32_t> hA = A;
        std::vector<uint32_t> hB = B;
        for (auto& val : hA) {
            val = h(val);
        }
        for (auto& val : hB) {
            val = h(val);
        }

        // do brute force
        auto cur_res = HAM(n, m, hA, hB);
        assert(cur_res.size() == dist.size());
        for (size_t i = 0; i < cur_res.size(); i++) {
            dist[i] = std::max(dist[i], (uint32_t)cur_res[i]);
        }
    }
}
