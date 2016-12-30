////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (sgdTrainer)
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
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"
#include "ModelLoadArguments.h"
#include "ModelSaveArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "TrainerArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"

// nodes
#include "LinearPredictorNode.h"

// trainers
#include "EvaluatingIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"
#include "SGDIncrementalTrainer.h"

// evaluators
#include "BinaryErrorAggregator.h"
#include "Evaluator.h"
#include "LossAggregator.h"

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedSGDIncrementalTrainerArguments sgdIncrementalTrainerArguments;
        common::ParsedMultiEpochIncrementalTrainerArguments multiEpochTrainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(multiEpochTrainerArguments);
        commandLineParser.AddOptionSet(sgdIncrementalTrainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if (trainerArguments.verbose)
        {
            std::cout << "Stochastic Gradient Descent Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        mapLoadArguments.defaultInputSize = dataLoadArguments.parsedDataDimension;
        model::DynamicMap map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto mappedDataset = common::GetMappedDataset(dataLoadArguments, map);
        auto mappedDatasetDimension = map.GetOutput(0).Size();

        // predictor type
        using PredictorType = predictors::LinearPredictor;

        // create sgd trainer
        auto trainer = common::MakeSGDIncrementalTrainer(mappedDatasetDimension, trainerArguments.lossArguments, sgdIncrementalTrainerArguments);

        // in verbose mode, create an evaluator and wrap the sgd trainer with an evaluatingTrainer
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator = nullptr;
        if (trainerArguments.verbose)
        {
            evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossArguments);
            trainer = std::make_unique<trainers::EvaluatingIncrementalTrainer<PredictorType>>(trainers::MakeEvaluatingIncrementalTrainer(std::move(trainer), evaluator));
        }

        // create multi epoch trainer
        trainer = trainers::MakeMultiEpochIncrementalTrainer(std::move(trainer), multiEpochTrainerArguments);

        // Train the predictor
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->Update(mappedDataset.GetAnyDataset());
        auto predictor = trainer->GetPredictor();

        // Print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // Save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            auto model = common::AppendNodeToModel<nodes::LinearPredictorNode, PredictorType>(map, *predictor);
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
