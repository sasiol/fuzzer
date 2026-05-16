#include "helpers.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

//Here can be found functions for:
//setFuzzMode
//setLogMode
//readFile
//writeFile
//shareMemory
//findTargetBinary
//printStatus

unsigned char* shm_map = nullptr;
int shm_id = -1;
fuzzMode fmode;
LogMode lmode;

std::vector<unsigned char> lastInterestingInput;

void setFuzzMode() {
    char fuzzChoice;

    std::cout << "Select Fuzzing mode:\n";
    std::cout << "1 = random\n";
    std::cout << "2 = coverage guided\n";

    if (!(std::cin >> fuzzChoice)) {
        fuzzChoice = '2';
    }

    if (fuzzChoice == '1')
        fmode = fuzzMode::RANDOM;
    else
        fmode = fuzzMode::GUIDED;
}

void setLogMode() {
    char logChoice;

    std::cout << "Select Logging mode:\n";
    std::cout << "1 = normal\n";
    std::cout << "2 = debug\n";

    if (!(std::cin >> logChoice)) {
        logChoice = '2';
    }

    if (logChoice == '2')
        lmode = LogMode::DEBUG;
    else
        lmode = LogMode::NORMAL;
}

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
// Save  file
void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write((char*)data.data(), data.size());
}


void shareMemory(){
    //id for allocated shared memory segment
    shm_id =shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT | 0600 );

    if (shm_id <0) {
        std::cerr << "shmget failed\n";
        exit(1);
    }
    //attach the shared memory segment to this process
    shm_map = (unsigned char*) shmat(shm_id, nullptr, 0);

    if (shm_map == (void*)-1) {
        std::cerr << "shmat failed\n";
        exit(1);
    }
}



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

std::string toPrintData(const std::vector<unsigned char>& data){
    std::string out;
    out.reserve(data.size());

    for (unsigned char c : data) {
        if (std::isprint(c))
            out += c;
        else
            out += '.';
    }

    return out;
}


void printStatus(int iteration,
                 int coverage,
                 int crashCount,
                 const std::string& mode,
                 const std::vector<unsigned char>& lastInput){

                    std::cout << "\r\033[2K"   // clears entire line
                    << "Iterations: " << iteration
                    << " | Mode: " << mode
                    << " | Coverage: " << coverage
                    << " | Crashes: " << crashCount
                    << " | Last : " << toPrintData(lastInput)
                    << std::flush;
}


