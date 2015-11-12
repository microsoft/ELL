// files.h

#pragma once

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

namespace utilities
{
	ifstream OpenIfstream(string filepath);
}