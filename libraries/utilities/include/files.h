// files.h

#pragma once

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;

namespace utilities
{
    ifstream OpenIfstream(string filepath);
    ofstream OpenOfstream(string filepath);
}