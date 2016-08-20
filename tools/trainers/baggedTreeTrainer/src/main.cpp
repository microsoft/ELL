////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (baggedTreeTrainer)
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
#include "SupervisedExample.h"

// common
#include "SortingTreeTrainerArguments.h"
#include "BaggingIncrementalTrainerArguments.h"
#include "TrainerArguments.h"
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h" 
#include "EvaluatorArguments.h"
#include "DataLoaders.h"
#include "MakeTrainer.h"
#include "MakeEvaluator.h"

// trainers
#include "SortingTreeTrainer.h"

// evaluators
#include "IncrementalEvaluator.h"

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
        common::ParsedBaggingIncrementalTrainerArguments baggingIncrementalTrainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(sortingTreeTrainerArguments);
        commandLineParser.AddOptionSet(baggingIncrementalTrainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if(trainerArguments.verbose)
        {
            std::cout << "Bagged Tree Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load dataset
        if(trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto rowDataset = common::GetRowDataset(dataLoadArguments);

        // predictor type
        using PredictorType = predictors::DecisionTreePredictor;

        // create evaluator
        std::shared_ptr<evaluators::IIncrementalEvaluator<PredictorType>> evaluator = nullptr;
        if(trainerArguments.verbose)
        {
            evaluator = common::MakeIncrementalEvaluator<PredictorType>(rowDataset.GetIterator(), evaluatorArguments, trainerArguments.lossArguments);
        }

        // create trainer
        auto baseTrainer = common::MakeSortingTreeTrainer(trainerArguments.lossArguments, sortingTreeTrainerArguments);
        auto trainer = trainers::MakeBaggingIncrementalTrainer(std::move(baseTrainer), baggingIncrementalTrainerArguments, evaluator);
        
        // train
        if(trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        auto trainSetIterator = rowDataset.GetIterator();
        trainer->Update(trainSetIterator);
        auto ensemble = trainer->GetPredictor();

        // print loss and errors
        if(trainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
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
