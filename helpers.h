#ifndef HELPERS_H
#define HELPERS_H

#pragma once
#include <vector>
#include <string>

inline constexpr int FUZZ_TIMEOUT_SECS = 3; //used if target program gets stuck

extern unsigned char* shm_map;
extern int shm_id;
inline constexpr int MAP_SIZE = 65536;

extern std::vector<unsigned char> lastInterestingInput;

std::string findTargetBinary();
std::vector<unsigned char> readFile(const std::string& filename);
void writeFile(const std::string& filename, const std::vector<unsigned char>& data);
std::string toPrintData(const std::vector<unsigned char>& data);
void printStatus(int iteration,
                 int coverage,
                 int crashCount,
                 const std::string& mode,
                 const std::vector<unsigned char>& lastInterestingInput);


void setFuzzMode();
enum class fuzzMode{
    RANDOM,
    GUIDED
};
extern fuzzMode fmode;

void setLogMode();
enum class LogMode {
    NORMAL,
    DEBUG
};

extern LogMode lmode;
#endif