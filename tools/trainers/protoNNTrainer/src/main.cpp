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
#include "ProtoNNPredictorNode.h"

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

        // create protonn trainer
        auto trainer = common::MakeProtoNNTrainer(mappedDataset.NumExamples(), mappedDataset.NumFeatures(), protoNNTrainerArguments);

        // predictor type
        using PredictorType = predictors::ProtoNNPredictor;

        // Train the predictor
        if (protoNNTrainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset(0, mappedDataset.NumExamples()));
        trainer->Update();

        predictors::ProtoNNPredictor predictor(trainer->GetPredictor());

        if (protoNNTrainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            std::cout << "Training accuracy\n";
            {
                auto accuracy = 0.0;
                auto truePositive = 0.0;
                auto exampleIterator = mappedDataset.GetExampleIterator();
                while (exampleIterator.IsValid())
                {
                    // get the Next example
                    const auto& example = exampleIterator.Get();
                    double label = example.GetMetadata().label;
                    const auto& dataVector = example.GetDataVector().ToArray();
                    auto prediction = predictor.Predict(dataVector);

                    if (prediction.label == label)
                        truePositive += 1;

                    exampleIterator.Next();
                }

                accuracy = truePositive / mappedDataset.NumExamples();
                std::cout << "\nAccuracy: " << accuracy << std::endl;
            }

            std::cout << std::endl;
        }

        // Save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            auto model = common::AppendNodeToModel<nodes::ProtoNNPredictorNode, PredictorType>(map, predictor);
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
