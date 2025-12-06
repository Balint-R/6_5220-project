#include <random>
#include <cstdint>
#include <vector>
#include <numeric>

// #include "/Users/mac/mylibrary/debug.hpp"

template <typename T>
auto generate_uniform_strings(size_t n, size_t m, size_t sigma, size_t seed){
    // Uniformly at random generate each character
	std::mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return std::uniform_int_distribution(a, b)(rng);
    };

    printf("Generating uniform test case... (n: %zu, m: %zu, sigma: %zu, seed: %zu)\n", n, m, sigma, seed);
    std::vector<T> A(n), B(m);
    for (size_t i = 0; i < n; i++){
        A[i] = randInt(0, sigma-1);
    }
    for (size_t i = 0; i < m; i++){
        B[i] = randInt(0, sigma-1);
    }
    return make_tuple(A, B);
}

template <typename T>
auto generate_skewed_strings(size_t n, size_t m, size_t sigma, size_t seed, double big_prob = 0.8) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> uni01(0.0, 1.0);
    auto randInt = [&](int a, int b) {
        return std::uniform_int_distribution<int>(a, b)(rng);
    };

    printf("Generating SKEWED test case... (n: %zu, m: %zu, sigma: %zu, seed: %zu, big_prob: %.3f)\n",
           n, m, sigma, seed, big_prob);

    std::vector<T> A(n), B(m);

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

    for (size_t i = 0; i < n; i++) {
        A[i] = gen_symbol();
        // cout << A[i] << " ";
    }
    for (size_t i = 0; i < m; i++) {
        B[i] = gen_symbol();
        // cout << B[i] << " ";
    }

    return std::make_tuple(A, B);
}

template <typename T>
auto generate_increasing_seq(size_t n, size_t m, size_t sigma, size_t seed){
    // Text:    (sigma/2)(sigma/2+1)(sigma/2+2)..
    // Pattern: 012...

    std::vector<T> A(n), B(m, 0);
    for (size_t i = 0; i < n-m; i+=1){
        // shuffle i to i+m-1
        // Random ordering of the alphabet
        // std::vector<int> ord(m);
        // std::iota(begin(ord), end(ord), 0);

        // for (size_t j = i; j < i+m; j++){
        //     A[j+ord[j-i]] = (j-i) % (sigma/2);
        // }

        // for (size_t j = i; j < i+m; j++){
        //     A[j] = ((i/m) % (sigma/2));
        // }
        A[i] = 1+ ( i % (sigma-1));
    }
    // for (size_t i = 0; i < m; i++){
    //     // B[i] = i % sigma;
    //     B[i] = sigma/2 + ((i) % (sigma/2));
    // }
    return make_tuple(A, B);
}
