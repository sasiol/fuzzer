#include "corpus.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::vector<std::vector<unsigned char>> corpus;

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

        corpus.push_back(data);
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
    return corpus[index];
}

//add new inputs to corpus (crash or new behaviour)
void addToCorpus( const std::vector<unsigned char>& data) {
    corpus.push_back(data);
}

