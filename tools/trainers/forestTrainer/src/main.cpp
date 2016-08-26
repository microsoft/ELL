////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (forestTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "CommandLineParser.h" 
#include "RandomEngines.h"
#include "Exception.h"

// dataset
#include "Example.h"

// common
#include "ForestTrainerArguments.h"
#include "TrainerArguments.h"
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "MakeTrainer.h"
#include "MakeEvaluator.h"

// trainers
#include "ForestTrainer.h"

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
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedHistogramForestTrainerArguments histogramForestTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(histogramForestTrainerArguments);
        
        // parse command line
        commandLineParser.Parse();
                
        if(trainerArguments.verbose)
        {
            std::cout << "Sorting Tree Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load dataset
        if(trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto rowDataset = common::GetRowDataset(dataLoadArguments, std::move(map));

        // create trainer
        auto trainer = common::MakeHistogramForestTrainer(trainerArguments.lossArguments, histogramForestTrainerArguments);

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        rowDataset.RandomPermute(rng);

        // train
        if(trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        auto dataIterator = rowDataset.GetIterator(0, 1000);
        trainer->Update(dataIterator);

        // print loss and errors
        if(trainerArguments.verbose)
        {
            //std::cout << "Finished training tree with " << tree.NumNodes() << " nodes." << std::endl; 

            // evaluate
            //auto evaluator = common::MakeEvaluator<predictors::DecisionTreePredictor>(rowDataset.GetIterator(), evaluators::EvaluatorParameters{1, false}, trainerArguments.lossArguments);
            //evaluator->Evaluate(tree);
            //std::cout << "Training error\n";
            //evaluator->Print(std::cout);
            //std::cout << std::endl;
        }

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
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
