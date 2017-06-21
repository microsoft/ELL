////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (linearTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearTrainerArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

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
#include "ModelSaveArguments.h"
#include "TrainerArguments.h"

// model
#include "DynamicMap.h"
#include "Model.h"

// nodes
#include "LinearPredictorNode.h"

// trainers
#include "MeanCalculator.h"

// evaluators
#include "Evaluator.h"

// predictors
#include "Normalizer.h"

// stl
#include <iostream>
#include <memory>

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedLinearTrainerArguments linearTrainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;

        commandLineParser.AddOptionSet(linearTrainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if (trainerArguments.verbose)
        {
            std::cout << "Linear Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        mapLoadArguments.defaultInputSize = dataLoadArguments.parsedDataDimension;
        auto map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto mappedDataset = common::GetMappedDataset(stream, map);
        auto mappedDatasetDimension = map.GetOutput(0).Size();

        // normalize data
        if (linearTrainerArguments.normalize)
        {
            if (trainerArguments.verbose) std::cout << "Sparisty-preserving data normalization ..." << std::endl;

            // find inverse absolute mean
            auto scaleVector = trainers::CalculateSparseTransformedMean(mappedDataset.GetAnyDataset(), [](data::IndexValue x) { return std::abs(x.value); });
            scaleVector.Transform([](double x) {return x > 0.0 ? 1.0 / x : 0.0; });

            // create normalizer
            auto coordinateTransformation = [&](data::IndexValue x) { return x.value * scaleVector[x.index]; };
            auto normalizer = predictors::MakeTransformationNormalizer<data::IterationPolicy::skipZeros>(coordinateTransformation);

            // apply normalizer to data
            auto normalizedDataset = common::GetMappedDataset(mappedDataset.GetExampleIterator(), normalizer);
            mappedDataset.Swap(normalizedDataset);
        }

        // predictor type
        using PredictorType = predictors::LinearPredictor;

        // create linear trainer
        std::unique_ptr<trainers::ITrainer<PredictorType>> trainer;
        switch (linearTrainerArguments.algorithm)
        {
        case LinearTrainerArguments::Algorithm::SGD:
            trainer = common::MakeSGDTrainer(trainerArguments.lossFunctionArguments, { linearTrainerArguments.regularization, linearTrainerArguments.randomSeedString });
            break;
        case LinearTrainerArguments::Algorithm::SparseDataSGD:
            trainer = common::MakeSparseDataSGDTrainer(trainerArguments.lossFunctionArguments, { linearTrainerArguments.regularization });
            break;
        case LinearTrainerArguments::Algorithm::SparseDataCenteredSGD:
        {
            auto mean = trainers::CalculateMean(mappedDataset.GetAnyDataset());
            trainer = common::MakeSparseDataCenteredSGDTrainer(trainerArguments.lossFunctionArguments, mean, { linearTrainerArguments.regularization });
            break;
        }
        case LinearTrainerArguments::Algorithm::SDCA:
        {
            trainer = common::MakeSDCATrainer(trainerArguments.lossFunctionArguments, { linearTrainerArguments.regularization, linearTrainerArguments.desiredPrecision, linearTrainerArguments.maxEpochs, linearTrainerArguments.permute, linearTrainerArguments.randomSeedString });
            break;
        }
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized algorithm type");
        }

        // create an evaluator
        auto evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossFunctionArguments);

        // Train the predictor
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset());
        
        for (size_t epoch = 0; epoch < trainerArguments.numEpochs; ++epoch)
        {
            trainer->Update();
            evaluator->Evaluate(trainer->GetPredictor());
        }
        
        predictors::LinearPredictor predictor(trainer->GetPredictor());
        predictor.Resize(mappedDatasetDimension);

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
            auto model = common::AppendNodeToModel<nodes::LinearPredictorNode, PredictorType>(map, predictor);
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
