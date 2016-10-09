////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (forestTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "RandomEngines.h"

// data
#include "Example.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "ForestTrainerArguments.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "ModelSaveArguments.h"
#include "TrainerArguments.h"

// trainers
#include "HistogramForestTrainer.h"
#include "SortingForestTrainer.h"

#include "ThresholdFinder.h" // TODO remove this

// lossFunctions
#include "LogLoss.h"
#include "SquaredLoss.h"

// stl
#include <iostream>
#include <stdexcept>

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedForestTrainerArguments forestTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(forestTrainerArguments);

        // parse command line
        commandLineParser.Parse();

        if (trainerArguments.verbose)
        {
            std::cout << "Sorting Tree Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load data set
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto dataset = common::GetDataset(dataLoadArguments);

        // XX
        // TODO
        
        auto tmpTrainer = trainers::HistogramForestTrainer<lossFunctions::SquaredLoss, trainers::LogitBooster, trainers::ExhaustiveThresholdFinder>(lossFunctions::SquaredLoss(), trainers::LogitBooster(), trainers::ExhaustiveThresholdFinder(), forestTrainerArguments);
        tmpTrainer.Update(dataset.GetAnyDataset());

        // create trainer
        std::unique_ptr<trainers::IIncrementalTrainer<predictors::SimpleForestPredictor>> trainer;
        if (true)
        {
            trainer = common::MakeSortingForestTrainer(trainerArguments.lossArguments, forestTrainerArguments);
        }
        else
        {
            trainer = common::MakeHistogramForestTrainer(trainerArguments.lossArguments, forestTrainerArguments);
        }

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        dataset.RandomPermute(rng);

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->Update(dataset.GetAnyDataset(0, 1000));

        // print loss and errors
        if (trainerArguments.verbose)
        {
            const auto& predictor = trainer->GetPredictor();
            std::cout << "Finished training forest with " << predictor->NumTrees() << " trees." << std::endl;

            // evaluate // TODO fix this
            // auto evaluator = common::MakeEvaluator<predictors::SimpleForestPredictor>(dataset.GetAnyDataset(), evaluators::EvaluatorParameters{1, false}, trainerArguments.lossArguments);
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
