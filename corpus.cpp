#include "corpus.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::vector<Input> corpus;

//score input based on coverage / times used
double score(const Input& in) {
    //std::cout << "SCORE\n";
    return (double)in.coverageCount / (1 + in.timesUsed);
}



// Read file into memory
std::vector<unsigned char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Error opening file\n";
        exit(1);
    }

    //Read all bytes into vector
    std::vector<unsigned char> data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return data;
}

void loadCorpus(const std::string& path) {
    //go through the files in the given folder
    for (const auto& entry : std::filesystem::directory_iterator(path)) {

        if (!entry.is_regular_file()) continue;
        std::vector<unsigned char> data = readFile(entry.path().string());

        //build input
        Input in;
        in.data = data;
        in.coverageCount = 0;
        in.timesUsed = 0;

        corpus.push_back(in); 
    }
    //for debug
    std::cout << "Loaded " << corpus.size() << " seeds\n";
}

//choose random input from corpus
Input& getRandomInput() {
    if (corpus.empty()) {
        std::cerr << "Corpus is empty!\n";
        exit(1);
    }

    int index = rand() % corpus.size();
    return corpus[index];
}

//add new inputs to corpus (crash or new behaviour)
void addToCorpus(const std::vector<unsigned char>& data, int coverageCount) {
    Input in;
    in.data = data;
    in.coverageCount = coverageCount;
    in.timesUsed = 0;

    std::cerr << "[NEW CORPUS ENTRY]\n";
    std::cerr << "corpus size: " << corpus.size() << "\n";

    corpus.push_back(in);
}

Input& getInput() {
    if (corpus.empty()) {
        std::cerr << "Corpus is empty!\n";
        exit(1);
    }

    double total = 0.0; //sum of all scores (probability line)

    for (auto& in : corpus) {
        double s = score(in);
        if (s < 1.0) s = 1.0; //make sure none are 0, cause they wont be selected
        total += s;
    }

    //get random point on the probability line
    double rPoint = ((double)rand() / RAND_MAX) * total;

    double seg = 0.0;

    //Find input that matches the rPoint
    for (auto& in : corpus) {
        double s = score(in);
        if (s < 1.0) s = 1.0;

        seg += s;

        if (seg >= rPoint) {
            in.timesUsed++;
            std::cerr << "[SELECTED INPUT]\n";
            std::cerr << "timesUsed: " << in.timesUsed << "\n";
            std::cerr << "coverageCount: " << in.coverageCount << "\n";
            std::cerr << "data: ";
                for (unsigned char c : in.data) {
                    if (std::isprint(c))
                        std::cerr << c;
                    else
                        std::cerr << ".";
                }
            std::cerr << "\n";
            return in;
        }
    }
    //fallback
    corpus.back().timesUsed++;
    return corpus.back();
}