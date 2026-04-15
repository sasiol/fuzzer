#ifndef CORPUS_H
#define CORPUS_H

#include <vector>
#include <string>

std::vector<unsigned char> readFile(const std::string& filename);
void loadCorpus(const std::string& path);
std::vector<unsigned char> getRandomInput();
void addToCorpus(const std::vector<unsigned char>& data);

#endif