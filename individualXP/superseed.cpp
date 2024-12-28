#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Function to parse a FASTA file and return a vector of sequence ID and sequence pairs
std::vector<std::pair<std::string, std::string>> parseFasta(const std::string& fastaPath) {
    std::vector<std::pair<std::string, std::string>> sequences;
    std::ifstream file(fastaPath);
    std::string line, id, sequence;

    while (std::getline(file, line)) {
        // Check if the line starts with '>', indicating a sequence ID
        if (line[0] == '>') {
            // Save the previous sequence before processing the new one
            if (!id.empty()) sequences.emplace_back(id, sequence);
            id = line.substr(1);  // Extract the sequence ID (omit '>')
            sequence.clear();     // Clear the sequence buffer
        } else {
            sequence += line;      // Append to the current sequence
        }
    }

    // Save the last sequence
    if (!id.empty()) sequences.emplace_back(id, sequence);
    return sequences;
}

// Function to split a sequence into 'n' seeds
std::vector<std::string> toseed(const std::string& seq, int n) {
    std::vector<std::string> result(n);
    for (size_t i = 0; i < seq.size(); ++i) {
        result[i % n] += seq[i];  // Distribute characters into 'n' parts
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "♡ Usage: ./superseed <fasta_path> <n> <output_file>" << std::endl;
        return 1;
    }

    std::string fastaPath = argv[1];  // Path to the input FASTA file
    int n = std::stoi(argv[2]);      // Number of seeds to generate
    std::string outputFilePath = argv[3];  // Output file path

    // Open the output file
    std::ofstream outputFile(outputFilePath);
    if (!outputFile) {
        std::cerr << "Error: Could not create output file at " << outputFilePath << std::endl;
        return 1;
    }

    // Parse the input FASTA file
    auto sequences = parseFasta(fastaPath);

    // Process each sequence and write the seeds to the output file
    for (const auto& [seqid, seq] : sequences) {
        auto seeds = toseed(seq, n);
        for (size_t x = 0; x < seeds.size(); ++x) {
            outputFile << ">" << seqid << "_part" << x + 1 << "\n" << seeds[x] << "\n";
        }
    }

    std::cout << "Output written to: " << outputFilePath << std::endl;
    return 0;
}
