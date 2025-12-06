#ifndef HEADER_UTILS
#define HEADER_UTILS

#include <cassert>
#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

const std::string FILE_STR = std::string(__FILE__);
const std::string DATA_DIR = FILE_STR.substr(0, FILE_STR.substr(0, FILE_STR.rfind('/')).rfind('/')) + "/data";
const std::string CACHE_DIR = FILE_STR.substr(0, FILE_STR.substr(0, FILE_STR.rfind('/')).rfind('/')) + "/cache";

// Function to parse a file with a text file to
// std::vector variable, blank lines and
// white spaces will be omitted
// Usage: `parse_text_file(file_path, destination_variables)`
template <typename T>
void parse_text_file(const std::string filename, std::vector<T>& parser_var) {
    char ch;
    std::fstream fin(filename, std::fstream::in);
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
auto parse_input_file(const std::string filename, std::vector<T>& A, std::vector<T>& B) {
    A.clear(); B.clear();
    size_t n, m;
    std::string line;
    std::string mod_filename = std::string(DATA_DIR) + "/" + filename;
    std::fstream fin(mod_filename, std::fstream::in);
    fin >> n >> m;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::getline(fin, line);
    for (char ch : line) {
        A.push_back((T)(unsigned char)ch);
    }
    std::getline(fin, line);
    for (char ch : line) {
        B.push_back((T)(unsigned char)ch);
    }
    return std::make_tuple(n, m);
}


//check cache
// if file doesn't exist, write answer to cache
// return answer from cache
std::string get_cache_name(const std::string filename){
    return CACHE_DIR + "/" + filename + ".ans";
}

bool check_output_cached(const std::string filename){
    return std::ifstream(get_cache_name(filename)).good();
}

template <typename T>
void write_output_to_cache(const std::string filename, const std::vector<T>& ref_solution){
    std::string cache_filename = get_cache_name(filename);
    std::fstream fout(cache_filename, std::fstream::out);
    for (const T& element : ref_solution){
        fout << element << " ";
    }
}

template <typename T>
auto get_answer_from_cache(const std::string filename, std::vector<T>& ref_solution){
    ref_solution.clear();
    std::string cache_filename = get_cache_name(filename);
    uint32_t num;
    std::fstream fin(cache_filename, std::fstream::in);
    while (fin >> num) {
        ref_solution.push_back((T) num);
    }
}

// Calculate the approximation ratio between reference solution and the approximation, 
// assuming the approximation achieves a lower than correct hamming distance for all matches
double approximation_ratio(const std::vector<uint32_t>& ref_solution, const std::vector<uint32_t>& approx_solution){
    assert(ref_solution.size() == approx_solution.size());
    size_t d = ref_solution.size();
    double final_ratio = 1;
    for(size_t i = 0; i < d; i++){
        final_ratio = std::min(final_ratio, ((double) approx_solution[i]) / (ref_solution[i]));
    }
    return 1 - final_ratio;
}

#endif
