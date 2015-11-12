// main.cpp

#include "JsonSerializer.h"
#include "compilable.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace compile;
using std::ifstream;
using std::cerr;
using std::cout;
using std::endl;
using std::dynamic_pointer_cast;
using std::runtime_error;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "usage: compile.exe <map file>\n";
        return 1;
    }

    // open file
    auto filepath = argv[1];
    ifstream fs(filepath);

    // check that it opened
    if (!fs.is_open())
    {
        cerr << "error reading file " << filepath << endl;
        return 1;
    }

    try
    {
        // read the map and compile out its description
        auto map = mappings::Io::read(fs);
        auto compilable_map = dynamic_pointer_cast<compilable>(map);
        compilable_map->compile(cout);

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
