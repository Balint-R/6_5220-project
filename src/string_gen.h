#include <cassert>
#include <random>
#include <vector>

using namespace std;

template <typename T>
pair<vector<T>, vector<T>> generate_uniform(int n, int m, int sigma, int seed){
	mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution(a, b)(rng);
    };

    vector<T> A(n), B(m);
    for (int i = 0; i < n; i++) A[i] = randInt(0, sigma-1);
    for (int i = 0; i < m; i++) B[i] = randInt(0, sigma-1);
    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_cyclic(int n, int m, int sigma, int seed){
    vector<T> A(n), B(m);
    for(int i = 0; i < n; i++) A[i] = i % sigma;
    for(int i = 0; i < m; i++) B[i] = i % sigma;
    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_k_difs(int n, int m, int sigma, int k, int seed){
    assert(0 < k && k <= sigma);
    mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution<int>(a, b)(rng);
    };

    vector<T> A(n), B(m);
    for(int i = 0; i < m; i++) B[i] = i % k;

    int bsz = m + k;
    for(int blk = 0; blk < (n + bsz-1)/bsz; blk++){
        vector<T> rand_arr(k);
        for(int i = 0; i < k; i++) rand_arr[i] = randInt(0, sigma-1);
        for(int j = 0; j < min(bsz, n - blk*bsz); j++) A[blk*bsz + j] = rand_arr[j % k];
    }

    return {A, B};
}
