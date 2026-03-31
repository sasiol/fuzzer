#include <cstdlib>
#include <iostream>

bool runTarget(const std::string& inputFile) {
    std::string command = "./tidy " + inputFile;

    int result = system(command.c_str());

    if (result != 0) {
        std::cout << "Crash detected!\n";
        return true;
    }

    return false;
}