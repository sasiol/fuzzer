#include "corpus.h"
#include "helpers.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cctype>

std::vector<Input> corpus;

//score input based on coverage / times used
double score(const Input& in) {
    //std::cout << "SCORE\n";
    return (double)in.coverageCount / (1 + in.timesUsed);
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
    corpus.push_back(in);
}

//choose input favouring new coverage
Input& getInput() {
    if (corpus.empty()) {
        std::cerr << "Corpus is empty!\n";
        exit(1);
    }

    double total = 0.0; //sum of all scores (probability line)

    for (auto& in : corpus) {
        double s = std::max(score(in), 0.1); //make sure none are 0, cause they wont be selected
        total += s;
    }

    //get random point on the probability line
    double rPoint = ((double)rand() / RAND_MAX) * total;

    double seg = 0.0;

    //Find input that matches the rPoint
    for (auto& in : corpus) {
        double s = std::max(score(in), 0.1);

        seg += s;

        if (seg >= rPoint) {
            in.timesUsed++;

            //debug part
            if (lmode == LogMode::DEBUG){
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
            }
            return in;
        }
    }
    //fallback
    corpus.back().timesUsed++;
    return corpus.back();
}