////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (protoNNTrainer)
//  Authors:  Suresh Iyengar
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
#include "TrainerArguments.h"
#include "ProtoNNTrainerArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"

// trainer
#include "ProtoNNTrainer.h"
//#include "EvaluatingIncrementalTrainer.h"

// predictor
#include "ProtoNNPredictor.h"

// nodes
//#include "ProtoNNPredictorNode.h"

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
        common::ParsedProtoNNTrainerArguments protoNNTrainerArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;
        common::ParsedTrainerArguments trainerArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(protoNNTrainerArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if (protoNNTrainerArguments.verbose)
        {
            std::cout << "ProtoNN Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load dataset
        if (protoNNTrainerArguments.verbose) std::cout << "Loading data ..." << std::endl;

        mapLoadArguments.defaultInputSize = dataLoadArguments.parsedDataDimension;
        auto map = common::LoadMap(mapLoadArguments);
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto mappedDataset = common::GetMappedDataset(stream, map);
        auto mappedDatasetDimension = map.GetOutput(0).Size();

        //auto dataset = common::GetDataset(dataLoadArguments);

        // create protonn trainer
        auto trainer = common::MakeProtoNNTrainer(mappedDataset.NumExamples(), mappedDataset.NumFeatures(), protoNNTrainerArguments);

        // predictor type
        using PredictorType = predictors::ProtoNNPredictor;

        // in verbose mode, create an evaluator and wrap the sgd trainer with an evaluatingTrainer
        std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator = nullptr;
        if (protoNNTrainerArguments.verbose)
        {
            evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossFunctionArguments);
            //trainer = std::make_unique<trainers::EvaluatingIncrementalTrainer<PredictorType>>(trainers::MakeEvaluatingIncrementalTrainer(std::move(trainer), evaluator));
        }

        // Train the predictor
        if (protoNNTrainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset(0, mappedDataset.NumExamples()));
        trainer->Update();

        // TODO: current evaluator compares double values, need one more overload for label comparison which are unsigned ints
        //evaluator->Evaluate(trainer->GetPredictor());

        predictors::ProtoNNPredictor predictor(trainer->GetPredictor());
        // Print loss and errors
        if (protoNNTrainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            std::cout << "Training error\n";
            //evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // TODO: Serialization of protonn node
        // Save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            //auto model = common::AppendNodeToModel<nodes::ProtoNNPredictorNode, PredictorType>(map, predictor);
            //common::SaveModel(model, modelSaveArguments.outputModelFilename);
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
