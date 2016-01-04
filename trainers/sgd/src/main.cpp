// main.cpp

// utilities
#include "files.h"
using utilities::OpenIfstream;
using utilities::OpenOfstream;

#include "CommandLineParser.h" 
using utilities::CommandLineParser;

#include "randomEngines.h"
using utilities::GetRandomEngine;

#include "MapLoadArguments.h" 
using utilities::ParsedMapLoadArguments;

#include "MapSaveArguments.h" 
using utilities::ParsedMapSaveArguments;

#include "DataLoadArguments.h" 
using utilities::ParsedDataLoadArguments;

#include "BinaryClassificationEvaluator.h"
using utilities::BinaryClassificationEvaluator;

// layers
#include "Map.h"
using layers::Map;

#include "CoordinateListFactory.h"
using layers::CoordinateListFactory;

// dataset
#include "SequentialLineIterator.h"
using dataset::SequentialLineIterator;

#include "SparseEntryParser.h"
using dataset::SparseEntryParser;

#include "MappedParser.h"
using dataset::MappedParser;

#include "DatasetLoader.h"
using dataset::RowDataset;
using dataset::DatasetLoader;

// optimization
#include "AsgdOptimizer.h"
using optimization::AsgdOptimizer;

// loss_functions
#include "HingeLoss.h"
#include "LogLoss.h"
using namespace loss_functions;

// command line arguments
#include "CommandLineArguments.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedMapLoadArguments mapLoadArguments(commandLineParser);
        ParsedDataLoadArguments dataLoadArguments(commandLineParser);
        ParsedMapSaveArguments mapSaveArguments(commandLineParser);
        ParsedSgdArguments sgdArguments(commandLineParser);
        
        // parse command line
        commandLineParser.ParseArgs();

        // create a map 
        Map map;

        // create a list of input coordinates for this map
        CoordinateList inputCoordinates;

        // create a dataset
        RowDataset data;

        // open data file
        ifstream dataFStream = OpenIfstream(dataLoadArguments.inputDataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // load data wihtout applying any map
            data = DatasetLoader::Load(lineIterator, sparseEntryParser);

            // create default map with single input layer
            map = Map(data.NumColumns());

            // create a coordinate list of this map
            inputCoordinates = CoordinateListFactory::Sequence(0, data.NumColumns()); 
        }
        else
        {
            // load map
            ifstream mapFStream = OpenIfstream(mapLoadArguments.inputMapFile);
            map = JsonSerializer::Load<Map>(mapFStream, "Base");

            // create list of output coordinates
            inputCoordinates = CoordinateListFactory::IgnoreSuffix(map, mapLoadArguments.inputMapIgnoreSuffix);
            
            // load data
            MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);
            data = DatasetLoader::Load(lineIterator, mappedParser);
        }

        // create loss function
        LogLoss loss;

        // create sgd trainer
        AsgdOptimizer optimizer(data.NumColumns());

        // create evaluator
        BinaryClassificationEvaluator evaluator;

        // create random number generator
        auto rng = GetRandomEngine(sgdArguments.dataRandomPermutationSeedString);

        // perform epochs
        for(int epoch = 0; epoch < sgdArguments.numEpochs; ++epoch)
        {
            // randomly permute the data
            data.RandPerm(rng);

            // iterate over the entire permuted dataset
            auto trainSetIterator = data.GetIterator();
            optimizer.Update(trainSetIterator, loss, sgdArguments.l2Regularization);

            // Evaluate
            auto evaluationIterator = data.GetIterator();
            evaluator.Evaluate(evaluationIterator, optimizer.GetPredictor(), loss);
        }

        // print loss and errors
        cout << "training error\n" << evaluator << endl;

        // update the map with the newly learned layers
        auto predictor = optimizer.GetPredictor();
        predictor.AddTo(map, inputCoordinates);

        // save map to output file
        if (mapSaveArguments.outputMapFile != "")
        {
            ofstream outputFStream = OpenOfstream(mapSaveArguments.outputMapFile);
            map.Serialize(outputFStream);
        }
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




