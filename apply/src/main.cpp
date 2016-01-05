// main.cpp

#include "CommandLineParser.h" 
using utilities::CommandLineParser;

#include "MapLoadArguments.h" 
using utilities::ParsedMapLoadArguments;

#include "DataLoadArguments.h" 
using utilities::ParsedDataLoadArguments;

#include "DataSaveArguments.h" 
using utilities::ParsedDataSaveArguments;

// layers
#include "Map.h"
using layers::Map;

#include "Coordinate.h"
using layers::CoordinateList;

// dataset
#include "SupervisedExample.h"
using dataset::RowDataset;

// common
#include "DatasetMapLoader.h"
using common::DatasetMapLoader;

// stl
#include <iostream>
using std::cerr;
using std::endl;

#include <stdexcept>
using std::runtime_error;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedMapLoadArguments mapLoadArguments(commandLineParser);
        ParsedDataLoadArguments dataLoadArguments(commandLineParser);
        ParsedDataSaveArguments dataSaveArguments(commandLineParser);

        // parse command line
        commandLineParser.ParseArgs();

        // create and load a dataset, a map, and a coordinate list
        RowDataset dataset;
        Map map;
        CoordinateList inputCoordinates;
        DatasetMapLoader::Load(dataLoadArguments, mapLoadArguments, dataset, map, inputCoordinates);

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}
