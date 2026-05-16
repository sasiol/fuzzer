#include "helpers.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

//Here can be found functions for:
//findTargetBinary
//printStatus



std::string findTargetBinary() {
    DIR* dir = opendir("./target");
    if (!dir) return "";

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        if (name == "." || name == "..") continue;

        // only allow files without extension (likely binaries)
        if (name.find('.') != std::string::npos) continue;

        std::string path = "./target/" + name;

        struct stat st;
        if (stat(path.c_str(), &st) == 0) {
            if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
                closedir(dir);
                return path;
            }
        }
    }

    closedir(dir);
    return "";
}





void printStatus(int iteration,
                 int coverage,
                 int crashCount,
                 const std::string& mode)
{
    std::cout << "\033[2J\033[H"; // for clearing the screen

    std::cout << "=== FUZZER STATUS ===\n";
    std::cout << "Iterations : " << iteration << "\n";
    std::cout << "Coverage   : " << coverage << "\n";
    std::cout << "Crashes    : " << crashCount << "\n";
    std::cout << "Mode       : " << mode << "\n";
}