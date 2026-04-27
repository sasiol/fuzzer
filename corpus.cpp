#include "corpus.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::vector<Input> corpus;

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
    for (const auto& entry : std::filesystem::directory_iterator(path)) {

        if (!entry.is_regular_file()) continue;
        std::vector<unsigned char> data = readFile(entry.path().string());

        Input in;
        in.data = data;
        in.coverageCount = 0;
        in.timesUsed = 0;

        corpus.push_back(in);
    }

    std::cout << "Loaded " << corpus.size() << " seeds\n";
}

//choose random input from corpus
std::vector<unsigned char> getRandomInput() {
    if (corpus.empty()) {
        std::cerr << "Corpus is empty!\n";
        exit(1);
    }

    int index = rand() % corpus.size();
    return corpus[index].data;
}

//add new inputs to corpus (crash or new behaviour)
void addToCorpus(const std::vector<unsigned char>& data, int coverageCount) {
    Input in;
    in.data = data;
    in.coverageCount = coverageCount;
    in.timesUsed = 0;

    corpus.push_back(in);
}

Input& getInput() {
    if (corpus.empty()) {
        std::cerr << "Corpus is empty!\n";
        exit(1);
    }

    double total = 0.0;

    for (auto& in : corpus) {
        double s = score(in);
        if (s < 1.0) s = 1.0;
        total += s;
    }

    double r = ((double)rand() / RAND_MAX) * total;

    double running = 0.0;

    for (auto& in : corpus) {
        double s = score(in);
        if (s < 1.0) s = 1.0;

        running += s;

        if (running >= r) {
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

    corpus.back().timesUsed++;
    return corpus.back();
}