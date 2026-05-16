#ifndef HELPERS_H
#define HELPERS_H

#pragma once
#include <string>

std::string findTargetBinary();
void printStatus(int iteration,
                 int coverage,
                 int crashCount,
                 const std::string& mode);
#endif