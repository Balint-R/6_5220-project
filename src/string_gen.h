#include <cassert>
#include <random>
#include <vector>

using namespace std;

template <typename T>
pair<vector<T>, vector<T>> generate_uniform_strings(int n, int m, int sigma, int seed){
    // Uniformly at random generate each character
	mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution(a, b)(rng);
    };

    printf("Generating uniform test case... (n: %d, m: %d, sigma: %d, seed: %d)\n", n, m, sigma, seed);
    vector<T> A(n), B(m);
    for (int i = 0; i < n; i++){
        A[i] = randInt(0, sigma-1);
    }
    for (int i = 0; i < m; i++){
        B[i] = randInt(0, sigma-1);
    }
    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_skewed_strings(int n, int m, int sigma, int seed, double big_prob = 0.8) {
    mt19937 rng(seed);
    uniform_real_distribution<double> uni01(0.0, 1.0);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution<int>(a, b)(rng);
    };

    printf("Generating SKEWED test case... (n: %d, m: %d, sigma: %d, seed: %d, big_prob: %.3f)\n",
           n, m, sigma, seed, big_prob);

    vector<T> A(n), B(m);

    auto gen_symbol = [&]() -> T {
        double u = uni01(rng);
        if (u < big_prob || sigma == 1) {
            return static_cast<T>(0); // generate the main symbol (0) with probability big_prob
        } else {
            // otherwsie pick uniformly from {1, 2, ..., sigma-1}
            int x = randInt(1, (int)sigma - 1);
            return static_cast<T>(x);
        }
    };

    for (int i = 0; i < n; i++) {
        A[i] = gen_symbol();
    }
    for (int i = 0; i < m; i++) {
        B[i] = gen_symbol();
    }

    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_increasing_seq(int n, int m, int sigma, int seed){
    // Text:    (sigma/2)(sigma/2+1)(sigma/2+2)..
    // Pattern: 012...

    vector<T> A(n), B(m, 0);
    for (int i = 0; i < n-m; i+=1){
        // shuffle i to i+m-1
        // Random ordering of the alphabet
        // vector<int> ord(m);
        // iota(begin(ord), end(ord), 0);

        // for (int j = i; j < i+m; j++){
        //     A[j+ord[j-i]] = (j-i) % (sigma/2);
        // }

        // for (int j = i; j < i+m; j++){
        //     A[j] = ((i/m) % (sigma/2));
        // }
        A[i] = 1+ ( i % (sigma-1));
    }
    // for (int i = 0; i < m; i++){
    //     // B[i] = i % sigma;
    //     B[i] = sigma/2 + ((i) % (sigma/2));
    // }
    return make_pair(A, B);
}

template <typename T>
pair<vector<T>, vector<T>> generate_all_difs(int n, int m, int sigma, int seed){
    assert(m % sigma == 0);
    int k = m/sigma;

    vector<T> A(n), B(m);
    for(int i = 0; i < sigma; i++){
        for(int j = 0; j < k; j++){
            B[i*k + j] = i;
        }
    }

    for(int i = 0; i < n; i++) A[i] = B[i % m];

    for(int i = 0; i < n/m; i++){
        int old_char = i % sigma;
        int new_char = i/sigma % sigma;
        for(int j = 0; j < k; j++) A[i*m + old_char*k] = new_char;
    }

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

    // dbgArr(A, n);
    // dbgArr(B, m);

    return {A, B};
}


template <typename T>
pair<vector<T>, vector<T>> generate_mblocks(int n, int m, int sigma, int seed){
    printf("Generating m-blocks test case... (n: %d, m: %d, sigma: %d, seed: %d)\n", n, m, sigma, seed);
    vector<T> A(n), B(m, 0);
    for (int i = 0; i < n; i++) {
        int block = n / m;
        A[i] = block % sigma;
    }

    return {A, B};
}
