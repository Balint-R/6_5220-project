#include "hamming_distance_bf.h"
#include "hamming_distance_proj.h"
#include "first_alg_with_heuristics.h"
#include "utils.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>

using namespace std;

constexpr size_t NUM_TESTS = 1;
size_t num_rounds;
const size_t SEED = 430298584;
mt19937 alg_rng(SEED);

// const auto FUNC = HammingDistanceBase;
const auto FUNC = HammingDistanceHeuristic_1;

template <typename T>
auto generate_strings(size_t n, size_t m, size_t sigma, size_t seed){
    // Uniformly at random generate each character
	mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution(a, b)(rng);
    };

    printf("Generating test case... (n: %zu, k: %zu, alpha: %zu, seed: %zu)\n", n, m, sigma, seed);
    vector<T> A(n), B(m);
    for (size_t i = 0; i < n; i++){
        A[i] = randInt(0, sigma-1);
    }
    for (size_t i = 0; i < m; i++){
        B[i] = randInt(0, sigma-1);
    }
    return make_tuple(A, B);
}

std::string get_test_name(int id){
	switch (id){
		case 0:
			return "Brute Force";
		case 1:
			return "Projection to Binary Alphabet";
		default:
			return "N/A";
	}
}

template <typename T>
double test(size_t n, size_t m, size_t sigma, double eps,
			const std::vector<T> &A, const std::vector<T> &B, 
			const std::optional<std::vector<T>> &ref_answer, int id=0) {
	std::cout << "\nTest name: " << get_test_name(id) << std::endl;

	vector<T> reference_answer;
	if (!ref_answer.has_value()) {
		cout << "\nRecalculating reference answer with brute force... " << endl;
		reference_answer = vector<T>(n-m+1, 0);
		HammingDistanceBF(n, m, A, B, reference_answer);
	}
	else {
		reference_answer = ref_answer.value();
	}

	// for (int i=0; i<reference_answer.size(); i++){
	//   cout << reference_answer[i] << " ";
	// }
	// cout << endl;

	double total_time = 0;
	for (size_t i = 0; i <= num_rounds; i++) {
		std::vector<T> result(n-m+1, 0); //initialize result array to all 0
		auto t1 = std::chrono::steady_clock::now();

		switch (id) {
			case 0:
				HammingDistanceBF(n, m, A, B, result);
				break;
			case 1:
				HammingDistanceProj(n, m, sigma, eps, A, B, result, alg_rng);
				break;
		}
		auto t2 = std::chrono::steady_clock::now();

		chrono::duration<double> dif_sec = t2 - t1;
		if (i == 0) {
			printf("Warmup round: %f\n", dif_sec.count());
		}
		else {
			printf("Round %zu time: %f\n", i, dif_sec.count());
			printf("Round %zu approximation ratio: %f\n", i, approximation_ratio(reference_answer, result));
			total_time += dif_sec.count();
		}
	}
	double average_time = total_time / num_rounds;
	printf("Average time: %f\n", total_time / num_rounds);
	return average_time;
}

void get_reference_solution(string filename, size_t n, size_t m, vector<uint32_t> &A,
	vector<uint32_t> &B, vector<uint32_t> &reference_solution){
	if (check_output_cached(filename)) {
		get_answer_from_cache(filename, reference_solution);
	}
	else {
		reference_solution = vector<uint32_t>(n-m+1, 0);
		HammingDistanceBF(n, m, A, B, reference_solution);
		write_output_to_cache(filename, reference_solution);
	}
}

void save_output_to_file(string filename, const vector<uint32_t> &dist){
	ofstream out_file(filename);
	out_file << dist.size() << "\n";
	for (auto num : dist) {
		out_file << num << " ";
	}
	out_file.close();
}


int main(int argc, char **argv){
	if (argc < 2) {
		printf(
			"Usage: ./testing_framework <mode> <mode_args>\n"
			"mode: real or synth\n"
			"mode_args: arguments specific to mode");
		exit(0);
	}
	std::string mode = argv[1];
	int id = 0;
	double eps = 0.01;
	if (mode == "synth"){
		if (argc < 7){
			printf(
				"Usage: ./testing_framework synth <n> <m> <eps> <sigma> <rounds> <id>\n"
				"n: length of the first string\n"
				"m: length of the second(pattern) string\n"
				"eps: desired approximation ratio\n"
				"sigma: alphabet size\n"
				"rounds: number of rounds\n"
				"id: algorithm to test\n");
			exit(0);
		}
		size_t n = atoi(argv[2]);
		size_t m = atoi(argv[3]);
		assert(n >= m);
		eps = atof(argv[4]);
		size_t sigma = atoi(argv[5]);
		num_rounds = atoi(argv[6]);
		if (argc >= 8) id = atoi(argv[7]);

		// Run synth data tests
		vector<uint32_t> A, B, reference_solution;
		std::tie(A, B) = generate_strings<uint32_t>(n, m, sigma, SEED);

		string filename = "synth_" + to_string(n) + "_" + to_string(m) + "_"
										+ to_string(sigma) + "_" + to_string(SEED);

		get_reference_solution(filename, n, m, A, B, reference_solution);

		test(n, m, sigma, eps, A, B, std::optional{reference_solution}, id);
	}
	else if (mode == "real"){
		if (argc < 5){
		printf(
			"Usage: ./testing_framework real <filename> <eps> <rounds> <id>\n"
			"filename: name of input file\n"
			"eps: desired approximation ratio"
			"rounds: number of rounds"
			"id: algorithm to test");
		exit(0);
		}
		std::string filename = argv[2];
		eps = atof(argv[3]);
		num_rounds = atoi(argv[4]);
		if (argc >= 6) id = atoi(argv[5]);

		vector<uint32_t> A, B, reference_solution;
		size_t n, m, sigma;
		std::tie(n, m) = parse_input_file(filename, A, B);
		assert(n == A.size());
		assert(m == B.size());
		sigma = 1 + max(*max_element(A.begin(), A.end()), *max_element(B.begin(), B.end()));

		get_reference_solution(filename, n, m, A, B, reference_solution);

		// save reference_solution to "filename.sol"
		string solution_filename = filename.replace(filename.find_last_of("."), filename.size(), ".sol");
		cout << "writing output to file: " << solution_filename << '\n';
		save_output_to_file(solution_filename, reference_solution);

		test(n, m, sigma, eps, A, B, std::optional{reference_solution}, id);
	}
	else {
		printf("Invalid mode. Should be 'synth' or 'real'.");
	}
}
