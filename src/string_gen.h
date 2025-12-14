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
pair<vector<T>, vector<T>> generate_mblocks_perturbed(int n, int m, int sigma, int seed, double diff_prob = 0.1){
    mt19937 rng(seed);
    auto randInt = [&](int a, int b) {
        return uniform_int_distribution<int>(a, b)(rng);
    };
    printf("Generating m-blocks with perturbations... (n: %d, m: %d, sigma: %d, seed: %d, diff_prob: %.3f)\n", n, m, sigma, seed, diff_prob);
    vector<T> A(n), B(m, 0);

    for (int i = 0; i < n; i++) {
        int block = i / m;
        if (randInt(0, 1000) < 1000 * diff_prob) {
            // perturbation
            A[i] = randInt(0, sigma - 1);
        } else {
            A[i] = block % sigma;
        }
    }

    for (int i = 0; i < m; i++) {
        B[i] = i % sigma;
    }

    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_geometric(int n, int m, int sigma, int seed, double prob = 0.5) {
    mt19937 rng(seed);
    uniform_real_distribution<double> uni01(0.0, 1.0);
    // auto randInt = [&](int a, int b) {
    //     return uniform_int_distribution<int>(a, b)(rng);
    // };

    printf("Generating GEOMETRIC test case... (n: %d, m: %d, sigma: %d, seed: %d, prob: %.3f)\n",
           n, m, sigma, seed, prob);

    vector<T> A(n), B(m);

    auto gen_symbol_inc = [&]() -> T {
        int cur_symbol = 0;
        while (cur_symbol < sigma - 1) {
            double u = uni01(rng);
            if (u < prob || sigma == 1) {
                return static_cast<T>(cur_symbol);
            } else {
                cur_symbol++;
            }
        }
        return cur_symbol;
    };

    auto gen_symbol_dec = [&]() -> T {
        int cur_symbol = sigma - 1;
        while (cur_symbol > 0) {
            double u = uni01(rng);
            if (u < prob || sigma == 1) {
                return static_cast<T>(cur_symbol);
            } else {
                cur_symbol--;
            }
        }
        return cur_symbol;
    };

    for (int i = 0; i < n; i++) {
        A[i] = gen_symbol_inc();
    }
    for (int i = 0; i < m; i++) {
        B[i] = gen_symbol_dec();
    }

    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_half_skewed(int n, int m, int sigma, int seed, double big_prob = 0.7) {
    assert(sigma >= 2);
    int half = max(1, sigma / 2);

    printf("Generating half-skewed test case... (n: %d, m: %d, sigma: %d, seed: %d, big_prob: %.3f)\n",
           n, m, sigma, seed, big_prob);


    mt19937 rng(seed);
    uniform_real_distribution<double> uni01(0.0, 1.0);
    uniform_int_distribution<int> big_dist(0, half - 1);
    uniform_int_distribution<int> small_dist(half, sigma - 1);

    auto gen_symbol = [&]() -> T {
        double u = uni01(rng);
        if (u < big_prob) {
            return static_cast<T>(big_dist(rng));
        } else {
            return static_cast<T>(small_dist(rng));
        }
    };

    vector<T> A(n), B(m);
    for (int i = 0; i < n; i++) A[i] = gen_symbol();
    for (int i = 0; i < m; i++) B[i] = gen_symbol();
    return {A, B};
}

template <typename T>
pair<vector<T>, vector<T>> generate_fake_binary(
    int n, int m, int sigma, int seed,
    double big_prob = 0.9) {

    assert(sigma >= 2);
    mt19937 rng(seed);

    printf("Generating fake binary test case... (n: %d, m: %d, sigma: %d, seed: %d, big_prob: %.3f)\n",
           n, m, sigma, seed, big_prob);

    int half = sigma / 2;
    vector<int> G0(half), G1(sigma - half);
    iota(G0.begin(), G0.end(), 0);
    iota(G1.begin(), G1.end(), half);

    uniform_int_distribution<int> bit(0, 1);
    uniform_real_distribution<double> uni01(0.0, 1.0);
    uniform_int_distribution<int> pickG0(0, (int)G0.size() - 1);
    uniform_int_distribution<int> pickG1(0, (int)G1.size() - 1);

    // main symbol for each group
    int a0 = G0[0];
    int b0 = G1[0];

    vector<T> A(n), B(m);
    vector<int> X(n), Y(m);
    for (int i = 0; i < n; i++) X[i] = bit(rng);
    for (int j = 0; j < m; j++) Y[j] = bit(rng);

    auto draw_from_group = [&](bool zero_group) -> T {
        double u = uni01(rng);
        if (u < big_prob) {
            return (T)(zero_group ? a0 : b0);  // generate main symbol with probability big_prob
        } else {
            // generate rare symbol
            if (zero_group) return (T) G0[pickG0(rng)];
            else return (T) G1[pickG1(rng)];
        }
    };

    for (int i = 0; i < n; i++) {
        A[i] = draw_from_group(X[i] == 0);
    }
    for (int j = 0; j < m; j++) {
        B[j] = draw_from_group(Y[j] == 0);
    }

    return {A, B};
}
