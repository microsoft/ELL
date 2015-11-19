// main.cpp

// utilities
#include "files.h"
#include "CommandLineParser.h"
using namespace utilities;

// treetools shared
#include "sharedArguments.h"
using namespace utilities;

// mappings
#include "mappings.h"
using namespace mappings;

_USE_DEFAULT_DESERIALIZER_  // use the default deserializer for mappings

// data iterators
//#include "SequentialLineIterator.h"
//#include "ParsingIterator.h"
//#include "SvmlightParser.h"
//#include "MappedParser.h"
//using namespace data_iterators;

#include <memory>
using std::dynamic_pointer_cast;

#include <iostream>
using std::cerr;
using std::endl;

#include <string>
using std::string;

int main(int argc, char* argv[])
{
    try
    {
        // parse the command line
        CommandLineParser cmd_instanceParser(argc, argv);
        ParsedSharedArguments shared_arguments(cmd_instanceParser);
        cmd_instanceParser.ParseArgs();

        // open map file
        ifstream map_fs = OpenIfstream(shared_arguments.mapFile);

        // open data file
        ifstream data_fs = OpenIfstream(shared_arguments.dataFile);

        // Load the model
        auto col = Io::ReadColumn(map_fs, shared_arguments.map_layers);

        //// we want to read the data file sequentially
        //SequentialLineIterator sli(data_fs);
        //
        //// get a parser for svmlight format
        //SvmlightParser sp;

        //// wrap the parser with a Mapping, to get a new parser
        //using mapped_svmlight_instanceParser = MappedParser<SvmlightParser>;
        //mapped_svmlight_instanceParser mp(sp, col);

        //// create an iterator that wraps the parser
        //using mapped_svmlight_parsing_iterator = ParsingIterator<SequentialLineIterator, mapped_svmlight_instanceParser>;
        //mapped_svmlight_parsing_iterator pi(sli, mp, col->GetOutputDim());

        //// process row by row
        //int counter = 0;
        //while (pi.IsValid())
        //{
        //    auto& x = pi.GetFeatureVector();
        //    auto y = pi.GetLabel();
        //    auto w = pi.GetWeight();

        //    // generate output and print it out TODO
        //    
        //    
        //    pi.Next();
        //    ++counter;
        //} 

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}



