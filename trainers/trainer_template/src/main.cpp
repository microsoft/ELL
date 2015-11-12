// main.cpp

#include "mappings.h"
#include "SequentialLineIterator.h"
#include "ParsingIterator.h"
#include "SvmlightParser.h"
#include "MappedParser.h"
#include "deserializer.h"
#include <stdexcept>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	// open file
	string data_filename = argv[1];
	string map_filename = argv[2];

	// open map file
	ifstream map_fs(map_filename);

	// check that it opened
	if(!map_fs.is_open())
	{
		cerr << "error reading map file from " << map_filename << endl;
		return 1;
	}

	// open data file
	ifstream data_fs(data_filename);

	// check that it opened
	if(!data_fs.is_open())
	{
		cerr << "error reading data file from" << data_filename << endl;
		return 1;
	}

	try
	{
	

	}
	catch (runtime_error e)
	{
		cerr << "runtime error: " << e.what() << std::endl;
	}

	return 0;
}




