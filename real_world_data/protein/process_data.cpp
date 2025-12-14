#include <bits/stdc++.h>
using namespace std;

string read_fasta_concat(const string &filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error: could not open FASTA file: " << filename << "\n";
        exit(1);
    }

    string line;
    string result;
    result.reserve(12000000); // approximate size of proteome: ~11 million

    while (getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '>') continue; // header line

        for (char c : line) {
            if (!isspace(static_cast<unsigned char>(c))) {
                result.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
            }
        }
    }

    return result;
}

string read_fasta_first_seq(const string &filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error: could not open FASTA file: " << filename << "\n";
        exit(1);
    }

    string line;
    string seq;
    bool in_seq = false;

    while (getline(in, line)) {
        if (line.empty()) continue;

        if (line[0] == '>') {
            if (in_seq) break; // already read first sequence, stop
            in_seq = true;
            continue;
        }

        if (in_seq) {
            for (char c : line) {
                if (!isspace(static_cast<unsigned char>(c))) {
                    seq.push_back(static_cast<char>(toupper(static_cast<unsigned char>(c))));
                }
            }
        }
    }

    return seq;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0]
             << " <proteome_fasta> <pattern_fasta> <out_file>\n";
        cerr << "Example:\n  " << argv[0]
             << " human.fasta titin.fasta input.in\n";
        return 1;
    }

    string proteome_fasta = argv[1];
    string pattern_fasta  = argv[2];
    string out_text_file  = argv[3];
    // string out_pattern_file = argv[4];

    cerr << "Reading proteome from " << proteome_fasta << "...\n";
    string T = read_fasta_concat(proteome_fasta);
    cerr << "Proteome length (aa): " << T.size() << "\n";

    cerr << "Reading pattern from " << pattern_fasta << "...\n";
    string P = read_fasta_first_seq(pattern_fasta);
    cerr << "Pattern length (aa): " << P.size() << "\n";

    {
        ofstream out(out_text_file);
        if (!out.is_open()) {
            cerr << "Error: could not open output file: " << out_text_file << "\n";
            return 1;
        }
        int n = T.size(), m = P.size();
        out << n << " " << m << "\n";
        out << T << "\n";
        out << P << "\n";
    }

    cerr << "Done. Wrote input to " << out_text_file << ".\n";

    return 0;
}
