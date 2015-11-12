// files.cpp

#include "files.h"
#include <stdexcept>

using std::runtime_error;

namespace utilities
{
	ifstream OpenIfstream(string filepath)
	{
		// open map file
		ifstream fs(filepath);

		// check that it opened
		if(!fs.is_open())
		{
			throw runtime_error("error openning file " + filepath);
		}

		return fs;
	}
}