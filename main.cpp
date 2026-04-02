#include "mutator.h"
#include "executor.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

int main() {
    int crashCount = 0;

    srand(time(0)); // seed randomness

    while (true) {

        //load seed file
        auto data = readFile("seed/seed.txt");

        //call mutator
        mutate(data);

        writeFile("mutated.bin", data);

        bool crashed = runTarget("mutated.bin");

        if (crashed) {
            std::cout << "found crashing input \n";

            //save results
            std::string crashFile = "crashes/crash_" + std::to_string(crashCount) + ".bin";

            writeFile(crashFile, data);

            crashCount++;

            break;
        }

        std::cout << "Mutation done\n";


    }

    return 0;

}
