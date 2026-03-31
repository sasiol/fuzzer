#include "mutator.h"
#include "executor.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

int main() {
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
            break;
        }

        std::cout << "Mutation done\n";

    

    //run target


    //detect crash


    //save results
    }

    return 0;

}
