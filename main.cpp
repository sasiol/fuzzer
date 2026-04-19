#include "mutator.h"
#include "executor.h"
#include "corpus.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

static const int MAP_SIZE = 65536; //define in common.h for example?
static bool globalCoverage[MAP_SIZE] = {0};

int main() {
    int crashCount = 0;

    srand(time(0)); 

    loadCorpus("seed");

    shareMemory();
    //setenv("SHM_ID", std::to_string(shm_id).c_str(), 1);
    while (true) {

        
        auto data = getRandomInput();

        //call mutator
        mutate(data);
        std::cout << "Mutation done\n";
        

        for (unsigned char c : data) {
            if (std::isprint(c))
                std::cout << c;
            else
                std::cout << ".";
        }

        std::cout << "\n";

        writeFile("mutated.bin", data);

        bool crashed = runTarget("mutated.bin");

        //check coverage
        bool newCoverage= false;

        for (int i = 0; i <MAP_SIZE; i++){
            if (shm_map[i] && !globalCoverage[i]) {
                globalCoverage[i] = 1;
                newCoverage = true;
            }
        }

        if (newCoverage) {
            std::cout << "NEW COVERAGE FOUND!\n";
            addToCorpus(data);
        }
        

        if (crashed) {
            std::cout << "found crashing input \n";

            //save results
            std::string crashFile = "crashes/crash_" + std::to_string(crashCount) + ".bin";

            writeFile(crashFile, data);
            addToCorpus(data);

            crashCount++;

            break;
        }
        


    }

    return 0;

}
