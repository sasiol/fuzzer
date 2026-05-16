#ifndef CORPUS_H
#define CORPUS_H

#include <vector>
#include <string>

struct Input {
    std::vector<unsigned char> data;
    int coverageCount;
    int timesUsed;
};

extern std::vector<Input> corpus;

double score(const Input& in);
void loadCorpus(const std::string& path);
Input& getRandomInput();
void addToCorpus(const std::vector<unsigned char>& data, int coverageCount);
Input& getInput();

#endif