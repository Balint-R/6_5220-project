#include <vector>
#include <fstream>


// Function to parse a file with a text file to
// std::vector variable, blank lines and
// white spaces will be omitted
// Usage: `parse_text_file(file_path, destination_variables)`
template <typename T>
void parse_text_file(const std::string filename, std::vector<T>& parser_var) {
    char ch;
    std::string mod_filename = std::string(DATA_DIR) + "/" + filename;
    std::fstream fin(mod_filename, std::fstream::in);
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