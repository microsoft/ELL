////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
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
#include "AppendNodeToModel.h"
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "EvaluatorArguments.h"
#include "ForestTrainerArguments.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"
#include "ModelLoadArguments.h"
#include "ModelSaveArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "TrainerArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"

// nodes
#include "ForestPredictorNode.h"

// trainers
#include "EvaluatingIncrementalTrainer.h"
#include "HistogramForestTrainer.h"
#include "MultiEpochIncrementalTrainer.h"
#include "SortingForestTrainer.h"

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

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedForestTrainerArguments forestTrainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;
        common::ParsedMultiEpochIncrementalTrainerArguments multiEpochTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
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

        // load map
        size_t numColumns = dataLoadArguments.parsedDataDimension;
        mapLoadArguments.defaultInputSize = numColumns;
        model::DynamicMap map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto mappedDataset = common::GetMappedDataset(stream, map);

        // predictor type
        using PredictorType = predictors::SimpleForestPredictor;

        // create trainer
        auto trainer = common::MakeForestTrainer(trainerArguments.lossArguments, forestTrainerArguments);

        // in verbose mode, create an evaluator and wrap the sgd trainer with an evaluatingTrainer
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator = nullptr;
        if (trainerArguments.verbose)
        {
            evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossArguments);
            trainer = std::make_unique<trainers::EvaluatingIncrementalTrainer<PredictorType>>(trainers::MakeEvaluatingIncrementalTrainer(std::move(trainer), evaluator));
        }

        // create multi epoch trainer
        trainer = trainers::MakeMultiEpochIncrementalTrainer(std::move(trainer), multiEpochTrainerArguments);

        // create random number generator
        auto rng = utilities::GetRandomEngine(trainerArguments.randomSeedString);

        // randomly permute the data
        mappedDataset.RandomPermute(rng);

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->Update(mappedDataset.GetAnyDataset());

        auto predictor = trainer->GetPredictor();
        // print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training forest with " << predictor.NumTrees() << " trees." << std::endl;

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // Save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            auto model = common::AppendNodeToModel<nodes::SimpleForestPredictorNode, PredictorType>(map, predictor);
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
