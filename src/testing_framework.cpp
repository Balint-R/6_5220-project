#include "ham_dist_bf.h"
#include "ham_dist_proj.h"
#include "ham_dist_sqrt.h"
#include "first_alg_with_heuristics.h"
#include "string_gen.h"
#include "utils.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

using namespace std;

int num_rounds;
const int SEED = 430298584;
mt19937 alg_rng(SEED);

const int NUM_ALGORITHMS = 5;

string get_test_name(int id){
	switch (id){
		case 0:
			return "Brute Force";
		case 1:
			return "Projection to Binary Alphabet";
		case 2:
			return "Sqrt";
        case 3:
            return "Heuristic 1: sum(bucket_mass)^2";
        case 4:
            return "Base Algorithm";
		default:
			return "N/A";
	}
}

template <typename T>
double test(int n, int m, int sigma, double eps, const vector<T> &A, const vector<T> &B,
			const vector<T> &ref_answer, int id=0) {
	cout << "\nTest name: " << get_test_name(id) << endl;
	double total_time = 0;
	for (int i = 0; i <= num_rounds; i++) {
		vector<T> result(n-m+1, 0); // initialize result array to all 0
		auto t1 = chrono::steady_clock::now();

		switch (id) {
			case 0:
				ham_dist_bf(n, m, A, B, result);
				break;
			case 1:
				ham_dist_proj(n, m, sigma, eps, A, B, result, alg_rng);
				break;
			case 2:
				ham_dist_sqrt(n, m, sigma, eps, A, B, result);
				break;
            case 3:
                HammingDistanceHeuristic_1(n, m, sigma, eps, A, B, result, alg_rng);
                break;
            case 4:
                HammingDistanceBase(n, m, sigma, eps, A, B, result, alg_rng);
                break;
			default:
				assert(false);
		}
		auto t2 = chrono::steady_clock::now();

		double dif_sec = chrono::duration<double>(t2 - t1).count();
		double approx_ratio = approximation_ratio(ref_answer, result);
		if (i == 0) {
			printf("Warmup round: %f\n", dif_sec);
			printf("Warmup round approximation ratio: %.6f\n", approx_ratio);
		}
		else {
			printf("Round %d time: %.6fs\n", i, dif_sec);
			total_time += dif_sec;
		}
		printf("Round %d approximation ratio: %.6f\n", i, approx_ratio);
	}
	double average_time = total_time / num_rounds;
	printf("Average time: %.6fs\n", total_time / num_rounds);
	return average_time;
}

double test_all(int n, int m, int sigma, double eps, const vector<uint32_t> &A, const vector<uint32_t> &B,
                           const vector<uint32_t> &ref_answer) {
    double total_time = 0;
    for (int id = 0; id < NUM_ALGORITHMS; id++) {
        cout << "\n==============================";
        double avg_time = test<uint32_t>(n, m, sigma, eps, A, B, ref_answer, id);
        total_time += avg_time;
    }
    return total_time;
}

void get_reference_solution(string filename, int n, int m, const vector<uint32_t> &A,
							const vector<uint32_t> &B, vector<uint32_t> &reference_solution){
	if (check_output_cached(filename)) {
		cerr << "Found cached solution" << endl;
		get_answer_from_cache(filename, reference_solution);
	}
	else {
		cerr << "Did not find cached solution" << endl;
		reference_solution.assign(n-m+1, 0);
		ham_dist_bf(n, m, A, B, reference_solution);
		write_output_to_cache(filename, reference_solution);
	}
}

int main(int argc, char **argv){
	if (argc < 2) {
		printf(
			"Usage: ./testing_framework <mode> <mode_args>\n"
			"mode: real or synth\n"
			"mode_args: arguments specific to mode\n");
		exit(1);
	}
	string mode = argv[1];
	int id = -1;
	if (mode == "synth") {
		if (argc < 7) {
			printf(
				"Usage: ./testing_framework synth <n> <m> <eps> <sigma> <rounds> <id>\n"
				"n: length of the first string\n"
				"m: length of the second(pattern) string\n"
				"eps: desired approximation ratio\n"
				"sigma: alphabet size\n"
				"rounds: number of rounds\n"
				"id: algorithm to test\n");
			exit(1);
		}
		int n = atoi(argv[2]);
		int m = atoi(argv[3]);
		assert(n >= m);
		double eps = atof(argv[4]);
		int sigma = atoi(argv[5]);
		num_rounds = atoi(argv[6]);
		if (argc >= 8) id = atoi(argv[7]);

		// Run synth data tests
		vector<uint32_t> A, B, reference_solution;
		std::tie(A, B) = generate_strings<uint32_t>(n, m, sigma, SEED);
		// std::tie(A, B) = generate_increasing_seq<uint32_t>(n, m, sigma, SEED);

		string filename = "synth_" + to_string(n) + "_" + to_string(m) + "_"
										+ to_string(sigma) + "_" + to_string(SEED);

		get_reference_solution(filename, n, m, A, B, reference_solution);

        if (id == -1) {
            test_all(n, m, sigma, eps, A, B, reference_solution);
        } else  {
		    test(n, m, sigma, eps, A, B, reference_solution, id);
        }
	}
	else if (mode == "real") {
		if (argc < 5) {
			printf(
				"Usage: ./testing_framework real <filename> <eps> <rounds> <id>\n"
				"filename: name of input file\n"
				"eps: desired approximation ratio\n"
				"rounds: number of rounds\n"
				"id: algorithm to test\n");
			exit(1);
		}
		string filename = argv[2];
		double eps = atof(argv[3]);
		num_rounds = atoi(argv[4]);
		if (argc >= 6) id = atoi(argv[5]);

		vector<uint32_t> A, B, reference_solution;
		int n, m, sigma;
		tie(n, m) = parse_input_file(filename, A, B);
		assert(n == (int) A.size());
		assert(m == (int) B.size());
		sigma = 1 + max(*max_element(A.begin(), A.end()), *max_element(B.begin(), B.end()));

		get_reference_solution(filename, n, m, A, B, reference_solution);

        if (id == -1) {
            test_all(n, m, sigma, eps, A, B, reference_solution);
        } else  {
            test(n, m, sigma, eps, A, B, reference_solution, id);
        }
	}
	else {
		printf("Invalid mode. Should be 'synth' or 'real'.\n");
		exit(1);
	}
}
