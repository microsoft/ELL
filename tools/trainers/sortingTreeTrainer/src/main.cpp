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
#include "Exception.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// dataset
#include "SupervisedExample.h"

// common
#include "SortingTreeTrainerArguments.h"
#include "TrainerArguments.h"
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MakeTrainer.h"
#include "MakeEvaluator.h"

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
        common::ParsedSortingTreeTrainerArguments sortingTreeTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(sortingTreeTrainerArguments);
        
        // parse command line
        commandLineParser.Parse();
                
        if(trainerArguments.verbose)
        {
            std::cout << "Sorting Tree Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(mapSaveArguments.outputModelFile);

        // load a model
        auto model = common::LoadModel(mapLoadArguments.modelLoadArguments);

        // get output coordinate list and create the map
        auto outputCoordinateList = layers::BuildCoordinateList(model, dataLoadArguments.parsedDataDimension, mapLoadArguments.coordinateListString);
        layers::Map map(model, outputCoordinateList);

        // load dataset
        if(trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto rowDataset = common::GetRowDataset(dataLoadArguments, std::move(map));

        // create trainer
        auto trainer = common::MakeSortingTreeTrainer(trainerArguments.lossArguments, sortingTreeTrainerArguments);

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        rowDataset.RandomPermute(rng);

        // train
        if(trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        auto dataIterator = rowDataset.GetIterator(0, 1000);
        auto tree = trainer->Train(dataIterator);

        // print loss and errors
        if(trainerArguments.verbose)
        {
            std::cout << "Finished training tree with " << tree.NumNodes() << " nodes." << std::endl; 

            // evaluate
            auto evaluator = common::MakeEvaluator<predictors::DecisionTreePredictor>(rowDataset.GetIterator(), evaluators::EvaluatorParameters{1, false}, trainerArguments.lossArguments);
            evaluator->Evaluate(tree);
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
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
    catch (utilities::Exception exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
