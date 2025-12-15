#include "ham_dist_bf.h"
#include "ham_dist_bf_fast.h"
#include "ham_dist_proj.h"
#include "ham_dist_proj_sc.h"
#include "ham_dist_sqrt.h"
#include "string_gen.h"
#include "utils.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

using namespace std;

const int SEED = 430298584;
mt19937 alg_rng(SEED);

const vector<int> ALG_IDS = {1, 2, 3, 4, 5};
const int NUM_ROUNDS[6] = {3, 3, 3, 3, 5, 5};
const int MAX_ROUNDS = *max_element(begin(NUM_ROUNDS), end(NUM_ROUNDS));
int override_rounds = -1;

string get_alg_name(int id){
	switch (id){
		case 0:
			return "Brute force";
		case 1:
			return "Brute force fast";
		case 2:
			return "Sqrt";
		case 3:
			return "Projection to 2/eps alphabet";
		case 4:
			return "Projection to 2/eps alphabet with short circuit";
		case 5:
			return "Projection to 2/eps alphabet with magic short circuit";
		default:
			return "N/A";
	}
}

string get_gen_name(int id){
    switch (id){
        case 0:
            return "Uniform";
        case 1:
            return "Cyclic";
        case 2:
            return "k difs";
        default:
            return "N/A";
    }
}

string get_gen_id_str(int id){
    switch (id){
        case 0:
            return "uniform";
        case 1:
            return "cyclic";
        case 2:
            return "k_difs";
        default:
            assert(false);
    }
}

struct TestCase {
    int n, m, sigma;
    vector<uint32_t> A, B, ref_sol;
};

void get_reference_solution(string filename, TestCase &tc){
	if (check_output_cached(filename)) {
		cerr << "Found cached solution" << endl;
		get_answer_from_cache(filename, tc.ref_sol);
	}
	else {
		cerr << "Did not find cached solution" << endl;
		tc.ref_sol.assign(tc.n - tc.m + 1, 0);
		ham_dist_sqrt(tc.n, tc.m, tc.sigma, tc.A, tc.B, tc.ref_sol);
		write_output_to_cache(filename, tc.ref_sol);
	}
}

vector<TestCase> gen_cases(int n, int m, int sigma, double eps, int num_cases, int type, int seed){
    /*
    Generate `num_cases` test cases of generator type `type`.
    The seeds start from `seed` and get incremented by 1 for each case.
    */
    char filename[100];
    vector<TestCase> cases(num_cases);

    for(int i = 0; i < num_cases; i++){
        switch (type) {
            case 0: {
                snprintf(filename, 100, "uniform_%d_%d_%d_%d", n, m, sigma, seed);
                tie(cases[i].A, cases[i].B) = generate_uniform<uint32_t>(n, m, sigma, seed);
                break;
            }
            case 1: {
                if(sigma > m) return {};
                snprintf(filename, 100, "cyclic_%d_%d_%d_%d", n, m, sigma, seed);
                tie(cases[i].A, cases[i].B) = generate_cyclic<uint32_t>(n, m, sigma, seed);
                break;
            }
            case 2: {
                double act_eps = 1 - (1 - eps)/(1 + eps);
                int k = ceil(1/act_eps - F_EPS) - 1;
                if(sigma > m || sigma < k*2) return {};
                snprintf(filename, 100, "k_difs_%d_%d_%d_%d_%d", n, m, sigma, k, seed);
                tie(cases[i].A, cases[i].B) = generate_k_difs<uint32_t>(n, m, sigma, k, seed);
                break;
            }
            default:
                assert(false);
        }

        cases[i].n = n;
        cases[i].m = m;
        cases[i].sigma = sigma;
        get_reference_solution(string(filename), cases[i]);
        seed++;
    }

    return cases;
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
TestStats test(const vector<TestCase> &cases, double eps, int alg_id) {
	printf("\nAlg name: %s\n\n", get_alg_name(alg_id).c_str());
    vector<double> times, ratios;
    const vector<uint32_t> dummy_sol(cases[0].n - cases[0].m + 1, 1e9);

    int num_rounds = NUM_ROUNDS[alg_id];
    if(override_rounds != -1) num_rounds = override_rounds;
    assert(num_rounds <= (int) cases.size());

    for (int i = 0; i < num_rounds; i++) {
        const auto &[n, m, sigma, A, B, ref_sol] = cases[i];
        vector<T> result(n - m + 1, 0); // initialize result array to all 0
        auto t1 = chrono::steady_clock::now();

		switch (alg_id) {
			case 0:
				ham_dist_bf(n, m, sigma, A, B, result);
				break;
			case 1:
				ham_dist_bf_fast(n, m, sigma, A, B, result);
				break;
			case 2:
				ham_dist_sqrt(n, m, sigma, A, B, result);
				break;
			case 3:
				ham_dist_proj(n, m, sigma, eps, A, B, result, dummy_sol, alg_rng);
				break;
			case 4:
				ham_dist_proj_sc(n, m, sigma, eps, A, B, result, alg_rng);
				break;
			case 5:
				ham_dist_proj(n, m, sigma, eps, A, B, result, ref_sol, alg_rng);
				break;
			default:
				assert(false);
		}
		auto t2 = chrono::steady_clock::now();

		double dif_sec = chrono::duration<double>(t2 - t1).count();
		double approx_ratio = approximation_ratio(ref_sol, result);

        printf("Round %d time: %.6fs\n", i, dif_sec);
		printf("Round %d approximation ratio: %.6f\n\n", i, approx_ratio);
        times.push_back(dif_sec);
        ratios.push_back(approx_ratio);
	}

    sort(times.begin(), times.end());
    sort(ratios.begin(), ratios.end());

    double avg_time = accumulate(times.begin(), times.end(), 0.0) / num_rounds;
    double median_time = times[times.size() / 2];
    double max_time = times.back();

    double avg_ratio = accumulate(ratios.begin(), ratios.end(), 0.0) / num_rounds;
    double median_ratio = ratios[ratios.size() / 2];
    double max_ratio = ratios.back();

    if(max_ratio - F_EPS > eps){
        printf("WARNING: max_ratio > eps: %.6f > %.6f\n", max_ratio, eps);
        fflush(stdout);
        // assert(false);
    }

    printf("Average time: %.6fs, Average approx ratio: %.6f, Max approx ratio: %.6f\n",
           avg_time, avg_ratio, max_ratio);

    return {avg_time, median_time, max_time, avg_ratio, median_ratio, max_ratio};
}

void test_all(const vector<TestCase> &cases, double eps) {
    for(int alg_id : ALG_IDS){
        cout << "==============================\n";
        test<uint32_t>(cases, eps, alg_id);
    }
}

// Helper lambda to run one configuration and append to CSV
void run_cases(const vector<TestCase> &cases, double eps, ofstream &csv){
    int num_cases = cases.size();

    for(int alg_id : ALG_IDS){
        int n = cases[0].n;
        int m = cases[0].m;
        int sigma = cases[0].sigma;

        printf("==============================\n");
        printf("n = %d, m = %d, sigma = %d, eps = %.3f, algo = %d\n", n, m, sigma, eps, alg_id);

        TestStats stats = test<uint32_t>(cases, eps, alg_id);

        csv << n << ","
            << m << ","
            << sigma << ","
            << eps << ","
            << alg_id << ","
            << "\"" << get_alg_name(alg_id) << "\"" << ","
            << num_cases << ","
            << stats.avg_time << ","
            << stats.median_time << ","
            << stats.max_time << ","
            << stats.avg_ratio << ","
            << stats.median_ratio << ","
            << stats.max_ratio << "\n";

        csv << flush;
        fflush(stdout);
        fflush(stderr);
    }
}

void run_synth_grid_to_csv(const string &csv_filename, int gen_id) {
    // Baselines values
    int base_n = 1e6;
    double base_m_frac = 0.50;
    int base_sigma = 100;
    double base_eps = 0.35;

    // Values to test
    vector<int> ns = {(int) 5e4, (int) 2e5, (int) 1e6};
    vector<double> m_fracs = {0.05, 0.10, 0.25, 0.50};
    vector<int> sigmas = {4, 10, 20, 40, 80, 160, 320, 640, 1280};
    vector<double> epsilons = {0.05, 0.10, 0.20, 0.35, 0.50};

    if(gen_id == 2){
        base_m_frac = 0.02;
        ns = {(int) 1e5, (int) 5e5, (int) 1e6};
        m_fracs = {0.01, 0.02, 0.04, 0.08};
        sigmas = {40, 80, 160, 320, 640, 1280};
    }

    ofstream csv(csv_filename.c_str());
    if (!csv.is_open()) {
        cerr << "Failed to open CSV file: " << csv_filename << endl;
        return;
    }

    // header
    csv << "n,m,sigma,eps,alg_id,algo_name,rounds,avg_time,median_time,max_time,";
    csv << "avg_ratio,median_ratio,max_ratio\n";

    int seed = 0;

    typedef tuple<int, double, int, double> Config;
    set<Config> done_configs;

    auto run_config = [&](int n, double m_frac, int sigma, double eps){
        Config config(n, m_frac, sigma, eps);
        if(done_configs.find(config) != done_configs.end()) return;

        int m = n * m_frac;
        const vector<TestCase> cases = gen_cases(n, m, sigma, eps, MAX_ROUNDS, gen_id, seed);
        if(cases.empty()) return;
        run_cases(cases, eps, csv);
        seed += 1000;

        done_configs.insert(config);
    };

    for (int n : ns) {
        run_config(n, base_m_frac, base_sigma, base_eps);
    }
    for (double mf : m_fracs) {
        run_config(base_n, mf, base_sigma, base_eps);
    }
    for (int sigma : sigmas) {
        run_config(base_n, base_m_frac, sigma, base_eps);
    }
    for (double eps : epsilons) {
        run_config(base_n, base_m_frac, base_sigma, eps);
    }

    csv.close();
}

void run_real_grid_to_csv(const string &csv_filename, const string &input_file) {
    vector<double> epsilons = {0.05, 0.10, 0.20, 0.50};

    ofstream csv(csv_filename.c_str());
    if (!csv.is_open()) {
        cerr << "Failed to open CSV file: " << csv_filename << endl;
        return;
    }

    // header
    csv << "dataset,n,m,sigma,eps,alg_id,algo_name,rounds,"
           "avg_time,median_time,max_time,"
           "avg_ratio,median_ratio,max_ratio\n";

    // read input file
    vector<uint32_t> A, B;
    int n, m, sigma;
    tie(n, m, sigma) = parse_input_file(input_file, A, B);
    assert(n == (int) A.size());
    assert(m == (int) B.size());

    fprintf(stderr, "n = %d, m = %d, sigma = %d\n", n, m, sigma);
    fprintf(stderr, "a_max = %d, b_max = %d\n", *max_element(A.begin(), A.end()), *max_element(B.begin(), B.end()));

    TestCase tc = {n, m, sigma, A, B, {}};
    get_reference_solution(input_file, tc);

    // repeat the same testcase num_rounds times
    vector<TestCase> cases(MAX_ROUNDS, tc);

    printf("Running input file %s\n\n", input_file.c_str());
    for (double eps : epsilons) {
        run_cases(cases, eps, csv);
    }

    csv.close();
}

int main(int argc, char **argv){
	if (argc < 2) {
		printf(
			"Usage: ./testing_framework <mode> <mode_args>\n"
			"mode: real, synth, synth_grid, real_grid\n"
			"mode_args: arguments specific to mode\n");
		exit(1);
	}
	string mode = argv[1];

    if (mode == "synth_grid") {
        if (argc < 3) {
            printf("Usage: ./testing_framework synth_grid <gen_id>\n");
            exit(1);
        }

        int gen_id = atoi(argv[2]);
        string filename = OUT_DIR + "/" + get_gen_id_str(gen_id) + ".csv";
        run_synth_grid_to_csv(filename, gen_id);
    }
    else if (mode == "real_grid") {
        if (argc < 4) {
            printf("Usage: ./testing_framework real_grid <input_file> <output_file_path>\n");
            printf("e.g. ./testing_framework real_grid titin_11m.in ../results/titin.csv\n");
            exit(1);
        }
        string input_file = argv[2];
        string output_file = argv[3];
        run_real_grid_to_csv(output_file, input_file);
    }
	else if (mode == "synth") {
		if (argc < 7) {
			printf(
				"Usage: ./testing_framework synth <n> <m> <eps> <sigma> <rounds> <alg_id> <gen_id>\n"
				"n: length of the first string\n"
				"m: length of the second(pattern) string\n"
				"eps: desired approximation ratio\n"
				"sigma: alphabet size\n"
				"rounds: number of rounds\n"
				"alg_id: algorithm to test\n"
				"gen_id: type of generator to use\n");
			exit(1);
		}
		int n = atoi(argv[2]);
		int m = atoi(argv[3]);
		assert(n >= m);
		double eps = atof(argv[4]);
		int sigma = atoi(argv[5]);
		override_rounds = atoi(argv[6]);

        int alg_id = -1;
		if (argc >= 8) alg_id = atoi(argv[7]);
        int gen_id = 0;
		if (argc >= 9) gen_id = atoi(argv[8]);

		// Run synth data tests
        vector<TestCase> cases = gen_cases(n, m, sigma, eps, override_rounds, gen_id, SEED);

        if (alg_id == -1) test_all(cases, eps);
        else test<uint32_t>(cases, eps, alg_id);
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
		override_rounds = atoi(argv[4]);

        int alg_id = -1;
		if (argc >= 6) alg_id = atoi(argv[5]);

		vector<uint32_t> A, B;
		int n, m, sigma;
		tie(n, m, sigma) = parse_input_file(filename, A, B);
		assert(n == (int) A.size());
		assert(m == (int) B.size());

        TestCase tc = {n, m, sigma, A, B, {}};
        get_reference_solution(filename, tc);
        vector<TestCase> cases = {tc};

        if (alg_id == -1) test_all(cases, eps);
        else test<uint32_t>(cases, eps, alg_id);
	}
	else {
		printf("Invalid mode. Should be 'synth' or 'real'.\n");
		exit(1);
	}
    return 0;
}
