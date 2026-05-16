#include "mutator.h"
#include "executor.h"
#include "corpus.h"
#include "helpers.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>

//Main fuzzing loop implementation


static bool globalCoverage[MAP_SIZE] = {0};
int iteration=0;
int globalCoverageCount = 0;

int main() {
    
    //ask user for what kind of fuzzing they want (random or coverage guided)
    //and what log mode they want (normal or debug)
    setFuzzMode();
    setLogMode();
    int crashCount = 0;

    srand(time(0)); 

    loadCorpus("seed"); 

    shareMemory(); //initialize shared memory segment
    //setenv("SHM_ID", std::to_string(shm_id).c_str(), 1);
    while (true) {

        iteration++; 
        //get the input to be used
        Input inCopy = (fmode == fuzzMode::RANDOM)
            ? getRandomInput()
            : getInput();

        auto data = inCopy.data;

        //mutate the input
        mutate(data);
        
        //for debugging the mutation
       if (lmode == LogMode::DEBUG) {
        std::cout << "Mutation done\n";
        for (unsigned char c : data) {
            if (std::isprint(c))
                std::cout << c;
            else
                std::cout << ".";
        }
        std::cout << "\n";
        }


        writeFile("mutated.bin", data);

        //run target program with the mutated data
        bool crashed = runTarget("mutated.bin");

        int coverageCount = 0;
        bool newCoverage = false;
        //track coverage by shared memory map
        for (int i = 0; i < MAP_SIZE; i++) {
            if (shm_map[i]) {
                coverageCount++;

                if (!globalCoverage[i]) {
                    globalCoverage[i] = 1;
                    globalCoverageCount++;
                    newCoverage = true;
                 }
            }
        }

        if (newCoverage) {
            //debug part for checking coverage growing
             if (lmode == LogMode::DEBUG) {
                std::cout << "NEW COVERAGE FOUND!\n";
                std::cout << "Global coverage: ";
                for (int i = 0; i < MAP_SIZE; i++) {
                    if (globalCoverage[i]) {
                        std::cout << i << " ";
                    }
                }
            std::cout << "\n";
            }
            
            lastInterestingInput = data;
            addToCorpus(data, coverageCount);
        }
        

        if (crashed) {
            //save results
            std::string crashFile = "crashes/crash_" + std::to_string(crashCount) + ".bin";

            writeFile(crashFile, data);
            //addToCorpus(data);

            crashCount++;
        }
        // show status
        if (lmode == LogMode::NORMAL){
            printStatus(
            iteration,
            globalCoverageCount,
            crashCount,
            (fmode == fuzzMode::RANDOM ? "random" : "guided"),
            lastInterestingInput
            );
        }
       std::memset(shm_map, 0, MAP_SIZE); 

    }
    //cleanup
    shmdt(shm_map);
    shmctl(shm_id, IPC_RMID, nullptr);
    return 0;

}
