#include "ham_dist_bf.h"
#include "ham_dist_proj.h"
#include "ham_dist_proj_sc.h"
#include "ham_dist_sqrt.h"
#include "first_alg_with_heuristics.h"
#include "string_gen.h"
#include "utils.h"
#include "HAM.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

using namespace std;

int num_rounds;
const int SEED = 430298584;
mt19937 alg_rng(SEED);

const int NUM_ALGORITHMS = 7;
const bool SKEWED = true; // whether to generate skewed test case
const double BIG_PROB = 0.8;

string get_test_name(int id){
	switch (id){
		case 0:
			return "Brute force";
		case 1:
			return "Projection to 2/eps alphabet";
		case 2:
			return "Sqrt";
        case 3:
            return "Heuristic 1: sum(bucket_mass)^2";
        case 4:
            return "Heuristic 2: sum(max freq over windows)^2";
        case 5:
            return "Heuristic 3: sum(weighted_bucket_mass)^2";
        case 6:
			return "Projection to 2/eps alphabet with short circuit";
		default:
			return "N/A";
	}
}

string get_gen_name(int id){
    switch (id){
        case 0:
            return "Uniform";
        case 1:
            return "Skewed";
        case 2:
            return "All diffs";
        case 3:
            return "k diffs";
        case 4:
            return "m-blocks";
        default:
            return "N/A";
    }
}

struct TestStats {
    double avg_time;
    double median_time;
    double max_time;
    double avg_ratio;
    double median_ratio;
    double max_ratio;
};

template <typename T>
TestStats test(int n, int m, int sigma, double eps, const vector<T> &A, const vector<T> &B,
               const vector<T> &ref_answer, int id = 0) {

	printf("\nTest name: %s\n\n", get_test_name(id).c_str());
	double sum_time = 0, sum_ratio = 0;
    vector<double> times, ratios;
    double max_ratio = 0;

    for (int i = 0; i <= num_rounds; i++) {
        vector<T> result(n - m + 1, 0); // initialize result array to all 0
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
                HammingDistanceHeuristic_2(n, m, sigma, eps, A, B, result, alg_rng);
                break;
            case 5:
                HammingDistanceHeuristic_3(n, m, sigma, eps, A, B, result, alg_rng);
                break;
            case 6:
				ham_dist_proj_sc(n, m, sigma, eps, A, B, result, alg_rng);
				break;
			default:
				assert(false);
		}
		auto t2 = chrono::steady_clock::now();

		double dif_sec = chrono::duration<double>(t2 - t1).count();
		double approx_ratio = approximation_ratio(ref_answer, result);
		if (i == 0) {
			printf("Warmup round: %fs\n", dif_sec);
			printf("Warmup round approximation ratio: %.6f\n", approx_ratio);
		}
		else {
			printf("Round %d time: %.6fs\n", i, dif_sec);
			sum_time += dif_sec;
            times.push_back(dif_sec);

            sum_ratio += approx_ratio;
        	max_ratio = max(max_ratio, approx_ratio);
            ratios.push_back(approx_ratio);
		}
		printf("Round %d approximation ratio: %.6f\n\n", i, approx_ratio);
	}

    sort(times.begin(), times.end());
    sort(ratios.begin(), ratios.end());

    double average_time = sum_time / num_rounds;
    double median_time = times[times.size() / 2];

    double average_ratio = sum_ratio / num_rounds;
    double median_ratio = ratios[ratios.size() / 2];

    printf("Average time: %.6fs, Average approx ratio: %.6f, Max approx ratio: %.6f\n",
           average_time, average_ratio, max_ratio);

    TestStats stats;
    stats.avg_time = average_time;
    stats.median_time = median_time;
    stats.max_time = times.back();

    stats.avg_ratio = average_ratio;
    stats.median_ratio = median_ratio;
    stats.max_ratio = max_ratio;
    return stats;
}


void test_all(int n, int m, int sigma, double eps, const vector<uint32_t> &A, const vector<uint32_t> &B,
                           const vector<uint32_t> &ref_answer) {
    for (int id = 0; id < NUM_ALGORITHMS; id++) {
        cout << "==============================\n";
        test<uint32_t>(n, m, sigma, eps, A, B, ref_answer, id);
    }
}

void get_reference_solution(string filename, int n, int m, const vector<uint32_t> &A,
							const vector<uint32_t> &B, vector<uint32_t> &reference_solution){
	// if (check_output_cached(filename)) {
	// 	cerr << "Found cached solution" << endl;
	// 	get_answer_from_cache(filename, reference_solution);
	// }
	// else {
		cerr << "Did not find cached solution" << endl;
		reference_solution.assign(n-m+1, 0);
		ham_dist_bf(n, m, A, B, reference_solution);
		write_output_to_cache(filename, reference_solution);
	// }
}

#include <fstream>
void run_synth_grid_to_csv_old(const std::string &csv_filename) {
    const int ns[] = {10000, 100000, 1000000};
    const double m_fracs[] = {0.05, 0.10, 0.20, 0.50};
    const int sigmas[] = {10, 100, 1000};
    const double epsilons[] = {0.10, 0.20, 0.50};

    const int num_ns = sizeof(ns) / sizeof(ns[0]);
    const int num_m_fracs = sizeof(m_fracs) / sizeof(m_fracs[0]);
    const int num_sigmas = sizeof(sigmas) / sizeof(sigmas[0]);
    const int num_eps = sizeof(epsilons) / sizeof(epsilons[0]);

    int old_rounds = num_rounds;
    num_rounds = 3;

    std::ofstream csv(csv_filename.c_str());
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV file: " << csv_filename << std::endl;
        return;
    }

    // header
    csv << "n,m,sigma,eps,algo_id,algo_name,rounds,avg_time,median_time,max_time,avg_ratio,median_ratio,max_ratio\n";

    for (int ni = 0; ni < num_ns; ++ni) {
        int n = ns[ni];

        for (int si = 0; si < num_sigmas; ++si) {
            int sigma = sigmas[si];

            for (int mi = 0; mi < num_m_fracs; ++mi) {
                int m = static_cast<int>(n * m_fracs[mi]);
                if (m > n) continue;

                std::cerr << "Generating test for n=" << n
                          << " m=" << m
                          << " sigma=" << sigma << std::endl;

                std::vector<uint32_t> A, B, reference_solution;

                // reference solution filename
                std::string filename = "all_difs_" + std::to_string(n) + "_" +
                                       std::to_string(m) + "_" +
                                       std::to_string(sigma) + "_" +
                                       std::to_string(SEED);

                if (SKEWED) {
                    std::tie(A, B) = generate_skewed_strings<uint32_t>(
                        n, m, sigma, SEED, BIG_PROB);
                } else {
                    std::tie(A, B) = generate_uniform_strings<uint32_t>(
                        n, m, sigma, SEED);
                }

                get_reference_solution(filename, n, m, A, B, reference_solution);

                for (int ei = 0; ei < num_eps; ++ei) {
                    double eps = epsilons[ei];

                    for (int algo_id = 0; algo_id < NUM_ALGORITHMS; ++algo_id) {
                        std::cout << "==============================\n";
                        std::cout << "n=" << n << " m=" << m
                                  << " sigma=" << sigma
                                  << " eps=" << eps
                                  << " algo=" << algo_id << std::endl;

                        TestStats stats = test<uint32_t>(
                            n, m, sigma, eps, A, B, reference_solution, algo_id);

                        csv << n << ","
                            << m << ","
                            << sigma << ","
                            << eps << ","
                            << algo_id << ","
                            << "\"" << get_test_name(algo_id) << "\"" << ","
                            << num_rounds << ","
                            << stats.avg_time << ","
                            << stats.median_time << ","
                            << stats.max_time << ","

                            << stats.avg_ratio << ","
                            << stats.median_ratio << ","
                            << stats.max_ratio << "\n";
                    }
                }
            }
        }
    }

    csv.close();
    num_rounds = old_rounds;
}

#include <fstream>
void run_synth_grid_to_csv(const std::string &csv_filename) {
    // Baselines values
    const int baseline_n = 1000000;
    const double baseline_m_frac = 0.20;
    const int baseline_sigma = 1000;
    const double baseline_eps = 0.10;

    // Values to test
    const std::vector<int> ns = {10000, 100000, 1000000};
    const std::vector<double> m_fracs = {0.05, 0.10, 0.20, 0.40, 0.50};
    const std::vector<int> sigmas = {4, 10, 20, 40, 80, 160, 320, 640, 1280};
    const std::vector<double> epsilons = {0.05, 0.10, 0.20, 0.50};

    // const int baseline_n = 1000;
    // const double baseline_m_frac = 0.10;
    // const int baseline_sigma = 40;
    // const double baseline_eps = 0.10;

    // const std::vector<int> ns = {1000, 2000};
    // const std::vector<double> m_fracs = {0.05, 0.10};
    // const std::vector<int> sigmas = {10, 20, 40};
    // const std::vector<double> epsilons = {0.10, 0.20};

    int old_rounds = num_rounds;
    num_rounds = 3; // or 5 if you want more stable averages

    std::ofstream csv(csv_filename.c_str());
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV file: " << csv_filename << std::endl;
        return;
    }

    // header
    csv << "n,m,sigma,eps,algo_id,algo_name,rounds,avg_time,median_time,max_time,avg_ratio,median_ratio,max_ratio\n";

    std::set<std::tuple<int,int,int,double>> seen;

    // Helper lambda to run one configuration and append to CSV
    auto run_config = [&](int n, double m_frac, int sigma, double eps) {
        int m = static_cast<int>(n * m_frac);
        if (m > n || m <= 0) return;

        auto key = std::make_tuple(n, m, sigma, eps);
        if (seen.count(key)) return;
        seen.insert(key);

        std::cerr << "Generating test for n=" << n
                  << " m=" << m
                  << " sigma=" << sigma
                  << " eps=" << eps << std::endl;

        std::vector<uint32_t> A, B, reference_solution;

        // reference solution filename (doesn't depend on eps)
        std::string filename = "all_difs_" + std::to_string(n) + "_" +
                               std::to_string(m) + "_" +
                               std::to_string(sigma) + "_" +
                               std::to_string(SEED);


        // if (SKEWED) {
        //     std::tie(A, B) = generate_skewed_strings<uint32_t>(
        //         n, m, sigma, SEED, BIG_PROB);
        // } else {
        //     std::tie(A, B) = generate_uniform_strings<uint32_t>(
        //         n, m, sigma, SEED);
        // }
        std::tie(A, B) = generate_mblocks_with_perturbations<uint32_t>(n, m, sigma, SEED);

        get_reference_solution(filename, n, m, A, B, reference_solution);

        for (int algo_id = 0; algo_id < NUM_ALGORITHMS; ++algo_id) {
            std::cout << "==============================\n";
            std::cout << "n=" << n << " m=" << m
                      << " sigma=" << sigma
                      << " eps=" << eps
                      << " algo=" << algo_id << std::endl;

            TestStats stats = test<uint32_t>(
                n, m, sigma, eps, A, B, reference_solution, algo_id);

                csv << n << ","
                    << m << ","
                    << sigma << ","
                    << eps << ","
                    << algo_id << ","
                    << "\"" << get_test_name(algo_id) << "\"" << ","
                    << num_rounds << ","
                    << stats.avg_time << ","
                    << stats.median_time << ","
                    << stats.max_time << ","

                    << stats.avg_ratio << ","
                    << stats.median_ratio << ","
                    << stats.max_ratio << "\n";
        }
    };

    for (int n : ns) {
        run_config(n, baseline_m_frac, baseline_sigma, baseline_eps);
    }
    for (double mf : m_fracs) {
        run_config(baseline_n, mf, baseline_sigma, baseline_eps);
    }
    for (int sigma : sigmas) {
        run_config(baseline_n, baseline_m_frac, sigma, baseline_eps);
    }
    for (double eps : epsilons) {
        run_config(baseline_n, baseline_m_frac, baseline_sigma, eps);
    }

    csv.close();
    num_rounds = old_rounds;
}

int main(int argc, char **argv){
    run_synth_grid_to_csv("../results/mblocks_perturbation.csv");
    return 0;

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

		int k = min<double>(sigma, ceil(1/eps) - 1);

		char buf[100];
		snprintf(buf, 100, "k_edges_%d_%d_%d_%d_%d", n, m, sigma, k, SEED);
		string filename(buf);

		// std::tie(A, B) = generate_k_difs<uint32_t>(n, m, sigma, k, SEED);

        // if (SKEWEr
		std::tie(A, B) = generate_mblocks_with_perturbations<uint32_t>(n, m, sigma, SEED);


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
    return 0;
}
