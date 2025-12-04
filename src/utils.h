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