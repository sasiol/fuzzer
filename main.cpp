#include "mutator.h"
#include "executor.h"
#include "corpus.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>

static const int MAP_SIZE = 65536; //define in common.h for example?
static bool globalCoverage[MAP_SIZE] = {0};

enum class Mode{
    RANDOM,
    GUIDED
};

Mode mode;


int main() {
    char choice;

    std::cout << "Select mode:\n";
    std::cout << "1 = random\n";
    std::cout << "2 = coverage guided\n";

    if (!(std::cin >> choice)) {
        choice = '2'; // default (mostly for pipeline)
    }

    if (choice == '1') mode = Mode::RANDOM;
    else mode = Mode::GUIDED;




    int crashCount = 0;

    srand(time(0)); 

    loadCorpus("seed"); 

    shareMemory(); //initialize shared memory segment
    //setenv("SHM_ID", std::to_string(shm_id).c_str(), 1);
    while (true) {

        //get the input to be used
        Input& in = (mode == Mode::RANDOM)
            ? getRandomInput()
            : getInput();
        auto data = in.data;

        //mutate the input
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

        int coverageCount = 0;
        for (int i = 0; i < MAP_SIZE; i++) {
            if (shm_map[i]) coverageCount++;
        }
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
            std::cout << "Global coverage: ";
            for (int i = 0; i < MAP_SIZE; i++) {
                if (globalCoverage[i]) {
                    std::cout << i << " ";
                }
            }
            std::cout << "\n";

            memset(shm_map, 0, MAP_SIZE);
            addToCorpus(data, coverageCount);
        }
        

        if (crashed) {
            std::cout << "found crashing input \n";

            //save results
            std::string crashFile = "crashes/crash_" + std::to_string(crashCount) + ".bin";

            writeFile(crashFile, data);
            //addToCorpus(data);

            crashCount++;

            break;
        }
        


    }

    return 0;

}
