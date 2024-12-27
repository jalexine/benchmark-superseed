#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "♡ pls use: " << argv[0] << " <kmer_file1> <kmer_file2>\n";
        return 1;
    }

    std::string file1 = argv[1];
    std::string file2 = argv[2];

    // read file1 into a set
    std::unordered_set<std::string> set1;
    set1.reserve(1000000); // reserve some space to optimize
    std::ifstream in1(file1);
    if(!in1.is_open()) {
        std::cerr << "nooo couldn't open file: " << file1 << std::endl;
        return 1;
    }
    
    std::string line;
    while(std::getline(in1, line)) {
        set1.insert(line);
    }
    in1.close();

    // now read file2 and:
    //  - count how many k-mers intersect
    //  - also collect them in another set to compute union
    std::unordered_set<std::string> set2;
    set2.reserve(1000000);
    std::ifstream in2(file2);
    if(!in2.is_open()) {
        std::cerr << "nooo couldn't open file: " << file2 << std::endl;
        return 1;
    }

    size_t intersection_count = 0;
    while(std::getline(in2, line)) {
        // if exists in set1, increment intersection
        if(set1.find(line) != set1.end()) {
            intersection_count++;
        }
        set2.insert(line);
    }
    in2.close();

    // union = set1.size() + set2.size() - intersection
    size_t union_count = set1.size() + set2.size() - intersection_count;

    // jaccard index
    double jaccard = 0.0;
    if(union_count > 0) {
        jaccard = static_cast<double>(intersection_count) / static_cast<double>(union_count);
    }

    std::cout << jaccard << std::endl;

    return 0;
}

