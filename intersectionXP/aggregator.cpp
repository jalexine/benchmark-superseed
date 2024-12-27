#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

// store jaccard in a map keyed by (f1, f2, n, k).
// single string key "f1/f2/n/k" 
struct JKKey {
    std::string f1, f2, n, k;
};
struct JKKeyHash {
    size_t operator()(JKKey const &k) const {
        // simple combine of hash values
        // not the best but good enough for demonstration
        std::hash<std::string> h;
        size_t res = 0;
        // just combine them in some way
        res ^= h(k.f1) + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        res ^= h(k.f2) + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        res ^= h(k.n)  + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        res ^= h(k.k)  + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        return res;
    }
};
struct JKKeyEq {
    bool operator()(JKKey const &a, JKKey const &b) const {
        return (a.f1 == b.f1 && a.f2 == b.f2 && a.n == b.n && a.k == b.k);
    }
};
std::string makeKeyStr(std::string f1, std::string f2, std::string n, std::string k){
    return f1 + "/" + f2 + "/" + n + "/" + k;
}

// same.. store skani in a map keyed by (f1, f2, n).
struct SkaniKey {
    std::string f1, f2, n;
};
struct SkaniKeyHash {
    size_t operator()(SkaniKey const &k) const {
        std::hash<std::string> h;
        size_t res = 0;
        res ^= h(k.f1) + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        res ^= h(k.f2) + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        res ^= h(k.n)  + 0x9e3779b97f4a7c15ULL + (res << 6) + (res >> 2);
        return res;
    }
};
struct SkaniKeyEq {
    bool operator()(SkaniKey const &a, SkaniKey const &b) const {
        return (a.f1 == b.f1 && a.f2 == b.f2 && a.n == b.n);
    }
};

// define some utility parsing
// e.g. jaccard file name: "ecoli_vs_ecoli2_N1_K31.txt"
bool parseJaccardFileName(const std::string &filename,
                          std::string &f1, std::string &f2,
                          std::string &n_val, std::string &k_val)
{
    // 1) remove directory path & extension
    size_t posTxt = filename.rfind(".txt");
    std::string base = (posTxt == std::string::npos) ? filename
                                                     : filename.substr(0, posTxt);

    auto parts = split(base, '_'); 

    if(parts.size() < 5) {
        return false;
    }


    f1 = parts[0];
    f2 = parts[2];

    if(parts[3].size() > 1 && parts[3][0] == 'N'){
        n_val = parts[3].substr(1); // "1"
    }

    if(parts[4].size() > 1 && parts[4][0] == 'K'){
        k_val = parts[4].substr(1);
    }

    return true;
}

bool parseSkaniFileName(const std::string &filename,
                        std::string &f1, std::string &f2,
                        std::string &n_val)
{
    size_t posAni = filename.rfind(".ani");
    std::string base = (posAni == std::string::npos) ? filename
                                                     : filename.substr(0, posAni);

    auto parts = split(base, '_');
    if(parts.size() < 4) {
        return false;
    }
    f1 = parts[0];
    f2 = parts[2];
    if(parts[3].size() > 1 && parts[3][0] == 'N'){
        n_val = parts[3].substr(1);
    }
    return true;
}

int main(int argc, char** argv){
    //  ./cpp_aggregator output.csv jaccard1 jaccard2 ... jaccardN --skani skani1 skani2 ...
    // we read argv[1] => outCSV
    // then read all arguments until we see "--skani"
    if(argc < 2){
        std::cerr << "♡ pls use: " << argv[0]
                  << " <output.csv> <jaccard files...> --skani <skani files...>\n";
        return 1;
    }

    std::string outCsv = argv[1];

    // parse the rest
    std::vector<std::string> jaccardPaths;
    std::vector<std::string> skaniPaths;

    bool readingSkani = false;
    for(int i = 2; i < argc; i++){
        std::string arg = argv[i];
        if(arg == "--skani"){
            readingSkani = true;
            continue;
        }
        if(!readingSkani){
            jaccardPaths.push_back(arg);
        } else {
            skaniPaths.push_back(arg);
        }
    }

    // 1) parse jaccard
    std::unordered_map< JKKey, std::string, JKKeyHash, JKKeyEq > jaccardDict;
    for(const auto &jf : jaccardPaths){
        // parse filename
        // we only want the basename
        size_t posSlash = jf.find_last_of("/\\");
        std::string fname = (posSlash == std::string::npos) ? jf
                                                            : jf.substr(posSlash+1);
        std::string f1, f2, n_val, k_val;
        if(!parseJaccardFileName(fname, f1, f2, n_val, k_val)){
            std::cerr << "Nooo couldn't parse jaccard filename: "
                      << fname << "\n";
            continue;
        }
        // read the single-line jaccard
        std::ifstream in(jf);
        if(!in){
            std::cerr << "Noooo can't open jaccard file: " << jf << "\n";
            continue;
        }
        std::string line;
        if(!std::getline(in, line)){
            line = "NA";
        }
        jaccardDict[{f1, f2, n_val, k_val}] = line;
    }

    // 2) parse skani
    std::unordered_map< SkaniKey, std::string, SkaniKeyHash, SkaniKeyEq > skaniDict;
    for(const auto &sf : skaniPaths){
        // parse filename
        size_t posSlash = sf.find_last_of("/\\");
        std::string fname = (posSlash == std::string::npos) ? sf
                                                            : sf.substr(posSlash+1);
        std::string f1, f2, n_val;
        if(!parseSkaniFileName(fname, f1, f2, n_val)){
            std::cerr << "Nooo couldn't parse skani filename: "
                      << fname << "\n";
            continue;
        }
        // read file, skip header line if "Ref_file"
        std::ifstream in(sf);
        if(!in){
            std::cerr << "Nooooo can't open skani file: " << sf << "\n";
            continue;
        }
        std::string ani_val = "NA";
        std::string line;
        while(std::getline(in, line)){
            if(line.empty()) continue;
            if(line.rfind("Ref_file", 0) == 0){
                // skip header
                continue;
            }
            // parse data line
            auto cols = split(line, '\t');
            if(cols.size() < 3){
                // it's space separated
                cols = split(line, ' ');
            }
            if(cols.size() >= 3){
                ani_val = cols[2]; // 3rd col is ANI
            }
            // read only first data line
            break;
        }
        skaniDict[{f1, f2, n_val}] = ani_val;
    }

    // 3) merge & write CSV
    std::ofstream out(outCsv);
    if(!out){
        std::cerr << "Error: cannot open output: " << outCsv << "\n";
        return 1;
    }
    out << "fasta1,fasta2,n,k,jaccard_score,skani_dist\n";

    // we want stable sorting => gather keys & sort
    std::vector<JKKey> allKeys;
    allKeys.reserve(jaccardDict.size());
    for(const auto &kv : jaccardDict){
        allKeys.push_back(kv.first);
    }
    // sort by f1,f2,n,k in lexical order
    std::sort(allKeys.begin(), allKeys.end(), [](auto &a, auto &b){
        if(a.f1 != b.f1) return a.f1 < b.f1;
        if(a.f2 != b.f2) return a.f2 < b.f2;
        if(a.n  != b.n ) return a.n  < b.n;
        return a.k < b.k;
    });

    for(const auto &key : allKeys){
        std::string jacc = jaccardDict[key];
        // find skani => ignoring 'k', so use (f1,f2,n)
        SkaniKey sk = {key.f1, key.f2, key.n};
        auto it = skaniDict.find(sk);
        std::string aniVal = (it != skaniDict.end()) ? it->second : "NA";
        out << key.f1 << ","
            << key.f2 << ","
            << key.n  << ","
            << key.k  << ","
            << jacc   << ","
            << aniVal << "\n";
    }

    out.close();
    return 0;
}

