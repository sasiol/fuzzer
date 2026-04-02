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

    int numMutations = rand() % 10 + 1; // change to use random next?

    for (int i = 0; i < numMutations; i++) {
        int x = rand() % 4;

        int index = rand() % data.size();   // pick random position

        switch(x){
            case 0: //flip bit
                data[index] ^= (1 << (rand()% 8));
                break;
            
            case 1:  //flip a byte
                 data[index] ^= 0xFF; 
                 break;

            case 2: //add byte
                data.insert(data.begin() + index, rand() % 256);
                break;

            case 3: //delete byte
                if (data.size() > 1)
                data.erase(data.begin()+index);
                break;
        }
       
    }
}

// Save  file
void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write((char*)data.data(), data.size());
}

