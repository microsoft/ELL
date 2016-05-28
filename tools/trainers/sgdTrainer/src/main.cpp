////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (sgdTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h" 
#include "CommandLineParser.h" 
#include "RandomEngines.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// dataset
#include "SupervisedExample.h"

// common
#include "sgdIncrementalTrainerArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "TrainerArguments.h"
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h" 
#include "EvaluatorArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MakeTrainer.h"
#include "MakeEvaluator.h"

// trainers
#include "SGDIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainer.h"
#include "EvaluatingIncrementalTrainer.h"

// evaluators
#include "Evaluator.h"
#include "BinaryErrorAggregator.h"
#include "LossAggregator.h"

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"

// stl
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <memory>

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
        common::ParsedSGDIncrementalTrainerArguments sgdIncrementalTrainerArguments;
        common::ParsedMultiEpochIncrementalTrainerArguments multiEpochTrainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(multiEpochTrainerArguments);
        commandLineParser.AddOptionSet(sgdIncrementalTrainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if(trainerArguments.verbose)
        {
            std::cout << "Stochastic Gradient Descent Trainer" << std::endl;
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
        auto rowDataset = common::GetRowDataset(dataLoadArguments, map);

        // create sgd trainer
        auto sgdIncrementalTrainer = common::MakeSGDIncrementalTrainer(outputCoordinateList.Size(), trainerArguments.lossArguments, sgdIncrementalTrainerArguments);

        // create evaluator
        std::shared_ptr<evaluators::IEvaluator<predictors::LinearPredictor>> evaluator = nullptr;
        if(trainerArguments.verbose)
        {
            evaluator = common::MakeEvaluator<predictors::LinearPredictor>(rowDataset.GetIterator(), evaluatorArguments, trainerArguments.lossArguments);
            sgdIncrementalTrainer = trainers::MakeEvaluatingIncrementalTrainer(std::move(sgdIncrementalTrainer), evaluator);
        }

        auto trainer = trainers::MakeMultiEpochIncrementalTrainer(std::move(sgdIncrementalTrainer), multiEpochTrainerArguments);

        // train
        if(trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        auto trainSetIterator = rowDataset.GetIterator();
        trainer->Update(trainSetIterator);
        auto predictor = trainer->GetPredictor();

        // print loss and errors
        if(trainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // add predictor to the model
        predictor->AddToModel(model, outputCoordinateList);

        // save the model
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
