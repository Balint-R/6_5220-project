#include "hamming_distance_bf.h"

#include <random>
#include <chrono>
#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

constexpr size_t NUM_TESTS = 1;
size_t num_rounds = 3;
size_t seed = 42;

mt19937 rng(430298584);
int randInt(int a, int b){return uniform_int_distribution(a, b)(rng);}

template <typename T>
auto generate_strings(size_t n, size_t m, size_t sigma, size_t seed){
    // Uniformly at random generate each character
    printf("Generating test case... (n: %zu, k: %zu, alpha: %zu, seed: %zu)\n", n, m, sigma, seed);
    std::vector<T> A(n), B(m);
    for (int i=0; i<n; i++){
        A[i] = randInt(0, sigma);
    }
    for (int i=0; i<m; i++){
        B[i] = randInt(0, sigma);
    }
    return std::make_tuple(A, B);
}


template <typename T>
double test(size_t n, size_t m, const std::vector<T> &A, const std::vector<T> &B,
            int id=0) {
  std::cout << "\nTest name: " << id << std::endl;
  double total_time = 0;
  for (size_t i = 0; i <= num_rounds; i++) {
    std::vector<T> result(n-m+1, 0); //initialize result array to all 0
    auto t1 = std::chrono::high_resolution_clock::now();
    HammingDistanceBF(n, m, A, B, result);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> s_float = t2 - t1;
    if (i == 0) {
      printf("Warmup round: %f\n", s_float.count());
    } else {
      printf("Round %zu: %f\n", i, s_float.count());
      total_time += s_float.count();
    }
  }
  double average_time = total_time / num_rounds;
  printf("Average time: %f\n", total_time / num_rounds);
  return average_time;
}

int main(int argc, char **argv){
    if (argc < 5){
        printf(
            "Usage: ./testing_framework <n> <m> <sigma> <rounds>\n"
            "n: length of the first string\n"
            "m: length of the second(pattern) string\n"
            "sigma: alphabet size\n"
            "rounds: number of rounds");
        exit(0);
    }
    size_t n = atoi(argv[1]);
    size_t m = atoi(argv[2]);
    assert(n >= m);
    size_t sigma = atoi(argv[3]);
    num_rounds = atoi(argv[4]);

    // Run all tests
    std::vector<uint32_t> A, B;
    std::tie(A, B) = generate_strings<uint32_t>(n, m, sigma, seed);
    test(n, m, A, B);
}
