//store file to memory
//flip, insert delete bytes
#include "mutator.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

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

// Mutate the data (flip random bytes)
void mutate(std::vector<unsigned char>& data) {
    if (data.empty()) return;

    int numMutations = rand() % 10; // change to use random next

    for (int i = 0; i < numMutations; i++) {
        int index = rand() % data.size();   // pick random position
        data[index] ^= 0xFF;      // flip 8 bits, better way to do this? ~ or std::bitset?
    }
}

// Save mutated file
void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write((char*)data.data(), data.size());
}

