#include "mutator.h"
#include "executor.h"
#include "corpus.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

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
