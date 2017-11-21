////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (sweepingSGDTrainer)
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
#include "AppendNodeToModel.h"
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "MapLoadArguments.h"
#include "ModelSaveArguments.h"
#include "ParametersEnumerator.h"
#include "TrainerArguments.h"

// trainers
#include "EvaluatingTrainer.h"
#include "SGDTrainer.h"
#include "SweepingTrainer.h"

// evaluators
#include "Evaluator.h"

// model
#include "Model.h"

// nodes
#include "LinearPredictorNode.h"

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
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);

        // parse command line
        commandLineParser.Parse();

        // default random seed
        const std::string defaultRandomSeed = "123";

        // manually define regularization parameters to sweep over
        std::vector<double> regularization{ 1.0e-0, 1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4, 1.0e-5, 1.0e-6 };
        std::vector<std::string> randomSeeds(regularization.size(), defaultRandomSeed);

        if (trainerArguments.verbose)
        {
            std::cout << "Sweeping Stochastic Gradient Descent Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        mapLoadArguments.defaultInputSize = dataLoadArguments.parsedDataDimension;
        auto map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto parsedDataset = common::GetDataset(stream);
        auto mappedDataset = common::TransformDataset(parsedDataset, map);
        auto mappedDatasetDimension = map.GetOutput(0).Size();

        // get predictor type
        using PredictorType = predictors::LinearPredictor<double>;
        using LinearPredictorNodeType = nodes::LinearPredictorNode<double>;

        // set up evaluators to only evaluate on the last update of the multi-epoch trainer
        evaluators::EvaluatorParameters evaluatorParameters{ 1, false };

        // create trainers
        auto generator = common::MakeParametersEnumerator<trainers::SGDTrainerParameters>(regularization, randomSeeds);
        std::vector<trainers::EvaluatingTrainer<PredictorType>> evaluatingTrainers;
        std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators;
        for (size_t i = 0; i < regularization.size(); ++i)
        {
            auto SGDTrainer = common::MakeSGDTrainer(trainerArguments.lossFunctionArguments, generator.GenerateParameters(i));
            evaluators.push_back(common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorParameters, trainerArguments.lossFunctionArguments));
            evaluatingTrainers.push_back(trainers::MakeEvaluatingTrainer(std::move(SGDTrainer), evaluators.back()));
        }

        // create meta trainer
        auto trainer = trainers::MakeSweepingTrainer(std::move(evaluatingTrainers));

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset());
        trainer->Update();
        PredictorType predictor(trainer->GetPredictor());
        predictor.Resize(mappedDatasetDimension);

        // print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            for (size_t i = 0; i < regularization.size(); ++i)
            {
                std::cout << "Trainer " << i << ":\n";
                evaluators[i]->Print(std::cout);
                std::cout << std::endl;
            }
        }

        // save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            auto model = common::AppendNodeToModel<LinearPredictorNodeType, PredictorType>(map, predictor);
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
