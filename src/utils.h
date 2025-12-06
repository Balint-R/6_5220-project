#ifndef HEADER_UTILS
#define HEADER_UTILS 1

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

constexpr string get_proj_dir(){
    string path = __FILE__;
    while(path.back() != '/') path.pop_back();
    path.pop_back();
    while(path.back() != '/') path.pop_back();
    path.pop_back();
    return path;
}

const string DATA_DIR = get_proj_dir() + "/data";
const string CACHE_DIR = get_proj_dir() + "/cache";

// Function to parse a file with a text file to
// vector variable, blank lines and
// white spaces will be omitted
// Usage: `parse_text_file(file_path, destination_variables)`
template <typename T>
void parse_text_file(const string filename, vector<T> &parser_var) {
    char ch;
    fstream fin(filename, fstream::in);
    while (fin >> ch) {
        unsigned char valid_ch = (unsigned char)(ch);
        parser_var.push_back((T)valid_ch);
    }
}


// Assumes the input file has the form
// n m
// text string
// pattern string
template <typename T>
pair<int, int> parse_input_file(const string filename, vector<T>& A, vector<T>& B) {
    A.clear(); B.clear();
    int n, m;
    string line;
    string mod_filename = string(DATA_DIR) + "/" + filename;
    ifstream fin(mod_filename);
    fin >> n >> m;
    fin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(fin, line);
    for (char ch : line) {
        A.push_back((T)(unsigned char)ch);
    }
    getline(fin, line);
    for (char ch : line) {
        B.push_back((T)(unsigned char)ch);
    }
    return {n, m};
}


//check cache
// if file doesn't exist, write answer to cache
// return answer from cache
inline string get_cache_name(const string filename){
    return CACHE_DIR + "/" + filename + ".ans";
}

inline bool check_output_cached(const string filename){
    return ifstream(get_cache_name(filename)).good();
}

template <typename T>
void write_output_to_cache(const string filename, const vector<T> &ref_solution){
    string cache_filename = get_cache_name(filename);

    // Ensure parent directories exist
    auto parent_path = filesystem::path(cache_filename).parent_path();
    filesystem::create_directories(parent_path);

    ofstream fout(cache_filename);
    assert(fout.good());

    int n = ref_solution.size();
    for(int i = 0; i < n; i++){
        fout << ref_solution[i] << " \n"[i == n-1];
    }
    fout.close();
}

template <typename T>
auto get_answer_from_cache(const string filename, vector<T> &ref_solution){
    ref_solution.clear();
    string cache_filename = get_cache_name(filename);
    uint32_t num;
    ifstream fin(cache_filename);
    while (fin >> num) {
        ref_solution.push_back((T) num);
    }
    fin.close();
}

// Calculate the approximation ratio between reference solution and the approximation
inline double approximation_ratio(const vector<uint32_t> &ref_solution,
                                  const vector<uint32_t> &approx_solution){
    assert(ref_solution.size() == approx_solution.size());
    int d = ref_solution.size();

    double final_ratio = 0;
    for(int i = 0; i < d; i++){
        int ref = ref_solution[i];
        int act = approx_solution[i];
        final_ratio = max(final_ratio, (double) abs(act - ref)/ref);
    }
    return final_ratio;
}

#endif
