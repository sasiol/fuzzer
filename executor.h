#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
extern int shm_id;
void shareMemory();
bool runTarget(const std::string& inputFile);

#endif
