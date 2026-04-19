#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
extern int shm_id;
extern unsigned char* shm_map;
void shareMemory();
bool runTarget(const std::string& inputFile);

#endif
