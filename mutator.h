#ifndef MUTATOR_H
#define MUTATOR_H

#include <vector>
#include <string>

std::vector<unsigned char> readFile(const std::string& filename);
void mutate(std::vector<unsigned char>& data);
void writeFile(const std::string& filename, const std::vector<unsigned char>& data);

#endif