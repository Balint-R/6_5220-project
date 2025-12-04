#include "hamming_distance_bf.h"
#include "utils.h"

#include <random>
#include <chrono>
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <algorithm>

constexpr size_t NUM_TESTS = 1;
size_t num_rounds = 3;
size_t seed = 430298584;

template <typename T>
auto generate_strings(size_t n, size_t m, size_t sigma, size_t seed){
    // Uniformly at random generate each character
    std::mt19937 rng(seed);
    auto randInt = [&rng](int a, int b) { // Capture x by value
        return std::uniform_int_distribution(a, b)(rng);
    };

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
double test(size_t n, size_t m, const std::vector<T> &A, const std::vector<T> &B, const std::optional<std::vector<T>>& ref_answer, 
            int id=0) {
  std::cout << "\nTest name: " << id << std::endl;

  std::vector<T> reference_answer;
  if (!ref_answer.has_value()){
    std::cout << "\nRecalculating reference answer with brute force... " << std::endl;
    reference_answer = std::vector<T>(n-m+1, 0);
    HammingDistanceBF(n, m, A, B, reference_answer);
  } else{
    reference_answer = ref_answer.value();
  }

  // for (int i=0; i<reference_answer.size(); i++){
  //   std::cout << reference_answer[i] << " ";
  // }
  // std::cout << std::endl;

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
      printf("Round %zu time: %f\n", i, s_float.count());
      printf("Round %zu approximation ratio: %f\n", i, approximation_ratio(reference_answer, result));
      total_time += s_float.count();
    }
  }
  double average_time = total_time / num_rounds;
  printf("Average time: %f\n", total_time / num_rounds);
  return average_time;
}

void get_reference_solution(std::string filename, size_t n, size_t m, std::vector<uint32_t>& A, std::vector<uint32_t>& B, 
  std::vector<uint32_t>& reference_solution){
  if (check_output_cached(filename)){
    get_answer_from_cache(filename, reference_solution);
  }
  else{
    reference_solution = std::vector<uint32_t>(n-m+1, 0);
    HammingDistanceBF(n, m, A, B, reference_solution);
    write_output_to_cache(filename, reference_solution);
  }
}


int main(int argc, char **argv){
  if (argc < 2){
    printf(
          "Usage: ./testing_framework <mode> <mode_args>\n"
          "mode: real or synth\n"
          "mode_args: arguments specific to mode");
      exit(0);
  }
  std::string mode = argv[1];
  if (mode == "synth"){
    if (argc < 6){
        printf(
            "Usage: ./testing_framework synth <n> <m> <sigma> <rounds>\n"
            "n: length of the first string\n"
            "m: length of the second(pattern) string\n"
            "sigma: alphabet size\n"
            "rounds: number of rounds");
        exit(0);
    }
    size_t n = atoi(argv[2]);
    size_t m = atoi(argv[3]);
    assert(n >= m);
    size_t sigma   = atoi(argv[4]);
    num_rounds = atoi(argv[5]);

    // Run synth data tests
    std::vector<uint32_t> A, B, reference_solution;
    std::tie(A, B) = generate_strings<uint32_t>(n, m, sigma, seed);

    std::string filename = "synth_" + std::to_string(n) + "_" + std::to_string(m) + "_" 
                                    + std::to_string(sigma) + "_" + std::to_string(seed);

    get_reference_solution(filename, n, m, A, B, reference_solution);

    test(n, m, A, B, std::optional{reference_solution});
  }
  else if (mode == "real"){
    if (argc < 4){
      printf(
          "Usage: ./testing_framework real <filename> <rounds>\n"
          "filename: name of input file\n"
          "rounds: number of rounds");
      exit(0);
    }
    std::string filename = argv[2];
    num_rounds = atoi(argv[3]);

    // Run real tests, assuming the format in file to be:
    // n m
    // text
    // pattern

    std::vector<uint32_t> A, B, reference_solution;
    size_t n,m;
    std::tie(n, m) = parse_input_file(filename, A, B);
    assert(n == A.size());
    assert(m == B.size());

    get_reference_solution(filename, n, m, A, B, reference_solution);

    test(n, m, A, B, std::optional{reference_solution});
  }
}
