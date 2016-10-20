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
#include "Dataset.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "EvaluatorArguments.h"
#include "ForestTrainerArguments.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "ModelSaveArguments.h"
#include "TrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"

// trainers
#include "HistogramForestTrainer.h"
#include "SortingForestTrainer.h"
#include "EvaluatingIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"

// lossFunctions
#include "LogLoss.h"
#include "SquaredLoss.h"

// model
#include "InputNode.h"
#include "Model.h"

// nodes
#include "ForestPredictorNode.h"

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
        common::ParsedEvaluatorArguments evaluatorArguments;
        common::ParsedMultiEpochIncrementalTrainerArguments multiEpochTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(multiEpochTrainerArguments);
        commandLineParser.AddOptionSet(forestTrainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

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

        // predictor type
        using PredictorType = predictors::SimpleForestPredictor;

        // create trainer
        auto trainer = common::MakeForestTrainer(trainerArguments.lossArguments, forestTrainerArguments);

        // in verbose mode, create an evaluator and wrap the sgd trainer with an evaluatingTrainer
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator = nullptr;
        if (trainerArguments.verbose)
        {
            evaluator = common::MakeEvaluator<PredictorType>(dataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossArguments);
            trainer = std::make_unique<trainers::EvaluatingIncrementalTrainer<PredictorType>>(trainers::MakeEvaluatingIncrementalTrainer(std::move(trainer), evaluator));
        }

        // create multi epoch trainer
        trainer = trainers::MakeMultiEpochIncrementalTrainer(std::move(trainer), multiEpochTrainerArguments);

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        dataset.RandomPermute(rng);

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->Update(dataset.GetAnyDataset());
        auto predictor = trainer->GetPredictor();

        // print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training forest with " << predictor->NumTrees() << " trees." << std::endl;

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            model::Model model;
            auto inputNode = model.AddNode<model::InputNode<double>>(1);
            model.AddNode<nodes::ForestPredictorNode<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor>>(inputNode->output, *predictor);
            common::SaveModel(model, modelSaveArguments.outputModelFilename);
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
