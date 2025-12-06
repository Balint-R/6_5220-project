#include <random>
#include <cstdint>
#include <vector>
#include <numeric>

template <typename T>
auto generate_strings(size_t n, size_t m, size_t sigma, size_t seed){
    // Uniformly at random generate each character
	std::mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return std::uniform_int_distribution(a, b)(rng);
    };

    printf("Generating test case... (n: %zu, m: %zu, sigma: %zu, seed: %zu)\n", n, m, sigma, seed);
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