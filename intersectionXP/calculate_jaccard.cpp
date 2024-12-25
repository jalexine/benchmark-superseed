#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>

using namespace std;

// read kmers from file
unordered_set<string> read_kmers(const string &filename) {
    unordered_set<string> kmers;
    ifstream file(filename);
    string kmer;

    while (file >> kmer) {
        kmers.insert(kmer);
    }

    return kmers;
}

// jaccard function
int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "♡ pls use: " << argv[0] << " <file1> <file2> <output_file>" << endl;
        return 1;
    }

    string file1 = argv[1];
    string file2 = argv[2];
    string output_file = argv[3];

    // read kmers from both files
    unordered_set<string> kmers1 = read_kmers(file1);
    unordered_set<string> kmers2 = read_kmers(file2);

    // intersection and union
    size_t intersection = 0;
    for (const auto &kmer : kmers1) {
        if (kmers2.count(kmer)) {
            intersection++;
        }
    }

    size_t union_size = kmers1.size() + kmers2.size() - intersection;

    // jaccard index
    double jaccard_index = union_size > 0 ? (double)intersection / union_size : 0.0;

    ofstream output(output_file);
    output << jaccard_index << endl;

    return 0;
}

