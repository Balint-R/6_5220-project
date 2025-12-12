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
pair<vector<T>, vector<T>> generate_cyclic(int n, int m, int sigma){
    assert(sigma <= m);
    vector<T> A(n), B(m);
    for(int i = 0; i < n; i++) A[i] = i % sigma;
    for(int i = 0; i < m; i++) B[i] = i % sigma;
    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_k_difs(int n, int m, int sigma, int k, int seed){
    assert(0 < k && k*2 <= sigma);
    assert(sigma <= m);
    mt19937 rng(seed);

    vector<T> A(n), B(m);
    for(int i = 0; i < m; i++) B[i] = i % sigma;

    for(int bl = 0; bl < n; bl += m){
        int br = min(bl + m, n);
        vector<T> mp(sigma), ord(sigma);
        for(int i = 0; i < sigma; i++) mp[i] = ord[i] = i;
        shuffle(ord.begin(), ord.end(), rng);
        for(int i = 0; i < k; i++) mp[ord[i*2]] = ord[i*2+1];

        for(int i = bl; i < br; i++) A[i] = mp[i % sigma];
    }

    return {A, B};
}
