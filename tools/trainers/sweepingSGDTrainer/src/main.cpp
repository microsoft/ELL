////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (sweepingSGDTrainer)
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
#include "SGDIncrementalTrainerArguments.h"
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
#include "ParametersEnumerator.h"

// trainers
#include "SGDIncrementalTrainer.h"
#include "SweepingIncrementalTrainer.h"
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

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(multiEpochTrainerArguments);
        commandLineParser.AddOptionSet(sgdIncrementalTrainerArguments);

        // parse command line
        commandLineParser.Parse();

        // manually define regularization parameters to sweep over
        std::vector<double> regularization{1.0e-0, 1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4, 1.0e-5, 1.0e-6};

        if(trainerArguments.verbose)
        {
            std::cout << "Sweeping Stochastic Gradient Descent Trainer" << std::endl;
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

        // get predictor type
        using PredictorType = predictors::LinearPredictor;

        // set up evaluators to only evaluate on the last update of the multi-epoch trainer
        evaluators::EvaluatorParameters evaluatorParameters{ multiEpochTrainerArguments.numEpochs, false };

        // create trainers
        auto generator = common::MakeParametersEnumerator<trainers::SGDIncrementalTrainerParameters>(regularization);
        std::vector<trainers::EvaluatingIncrementalTrainer<PredictorType>> evaluatingTrainers;
        std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators;
        for(uint64_t i = 0; i < regularization.size(); ++i)
        {
            auto sgdIncrementalTrainer = common::MakeSGDIncrementalTrainer(outputCoordinateList.Size(), trainerArguments.lossArguments, generator.GenerateParameters(i));
            evaluators.push_back(common::MakeEvaluator<PredictorType>(rowDataset.GetIterator(), evaluatorParameters, trainerArguments.lossArguments));
            evaluatingTrainers.push_back(trainers::MakeEvaluatingIncrementalTrainer(std::move(sgdIncrementalTrainer), evaluators.back()));
        }

        // create meta trainer
        auto trainer = trainers::MakeSweepingIncrementalTrainer(std::move(evaluatingTrainers), multiEpochTrainerArguments);

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
            for(uint64_t i = 0; i<regularization.size(); ++i)
            {
                std::cout << "Trainer " << i << ":\n";
                evaluators[i]->Print(std::cout);
                std::cout << std::endl;
            }
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
