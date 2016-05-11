////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (sortingTreeLearner)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "CommandLineParser.h" 
#include "RandomEngines.h"
#include "BinaryClassificationEvaluator.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// dataset
#include "SupervisedExample.h"

// common
#include "TrainerArguments.h"
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h" 
#include "DataLoaders.h"
#include "LoadModel.h"

// trainers
#include "SortingTreeTrainer.h"

// lossFunctions
#include "SquaredLoss.h"
#include "LogLoss.h"

// stl
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapSaveArguments mapSaveArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        
        // parse command line
        commandLineParser.Parse();

        if(trainerArguments.verbose) std::cout << "Sorting Tree Learner" << std::endl;
        
        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(mapSaveArguments.outputModelFile);

        // load a model
        auto model = common::LoadModel(mapLoadArguments);

        // get output coordinate list and create the map
        auto outputCoordinateList = layers::BuildCoordinateList(model, dataLoadArguments.parsedDataDimension, mapLoadArguments.coordinateListString);
        layers::Map map(model, outputCoordinateList);

        // load dataset
        if(trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto rowDataset = common::GetRowDataset(dataLoadArguments, std::move(map));

        // create sgd trainer
        lossFunctions::SquaredLoss loss;
        trainers::SortingTreeTrainer<lossFunctions::SquaredLoss> sortingTreeLearner(loss);
        
        // create evaluator
        utilities::BinaryClassificationEvaluator<predictors::DecisionTreePredictor, lossFunctions::SquaredLoss> evaluator;

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        rowDataset.RandomPermute(rng);

        // train
        if(trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        auto dataIterator = rowDataset.GetIterator(0, 30);
        auto tree = sortingTreeLearner.Train(dataIterator);

        // print loss and errors
        if(trainerArguments.verbose)
        {
            std::cout << "Finished training tree with " << tree.NumNodes() << " nodes." << std::endl; 

            auto evaluationIterator = rowDataset.GetIterator(0, 30);
            evaluator.Evaluate(evaluationIterator, tree, loss);
            std::cout << "Training error\n" << evaluator << std::endl;
        }

        // add tree to model
        tree.AddToModel(model, outputCoordinateList);

        // output map
        model.Save(outStream);

    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (std::runtime_error exception)
    {
        std::cerr << "runtime error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
