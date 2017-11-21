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
#include "Example.h"

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

// math
#include "MatrixOperations.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"

// trainer
#include "ProtoNNTrainer.h"

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
        auto parsedDataset = common::GetDataset(stream);
        auto mappedDataset = common::TransformDataset(parsedDataset, map);

        // The problem is NumFeatures returns a random number from sparse dataset depending on the number of trailing zeros it
        // has skipped.Is if the user did NOT specify - dd auto and instead provided a real input size like - dd 784 then we use
        // that number instead.
        auto dimension = mapLoadArguments.defaultInputSize != 0 ? mapLoadArguments.defaultInputSize : mappedDataset.NumFeatures();

        // now optionally scale the inputs if required
        if (dataLoadArguments.scale != 1)
        {
            auto numExamples = mappedDataset.NumExamples();
            for (size_t rowIndex = 0; rowIndex < numExamples; ++rowIndex)
            {
                auto& example = mappedDataset[rowIndex];
                auto data = math::ColumnVector<double>(example.GetDataVector().ToArray());
                data *= dataLoadArguments.scale;
                auto updated = data::AutoDataVector(data.ToArray());
                mappedDataset[rowIndex] = data::AutoSupervisedExample(std::move(updated), example.GetMetadata());
            }
        }

        // create protonn trainer
        auto trainer = common::MakeProtoNNTrainer(mappedDataset.NumExamples(), dimension, protoNNTrainerArguments);

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

            size_t test_index = 0;
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

                    auto maxElement = std::max_element(prediction.GetDataPointer(), prediction.GetDataPointer() + prediction.Size());
                    auto maxLabelIndex = maxElement - prediction.GetDataPointer();
                    if (maxLabelIndex == label)
                    {
                        truePositive += 1;
                    }
                    else if (protoNNTrainerArguments.verbose)
                    {
                        std::cout << "Test " << test_index << " failed: expecting label " << label << " and got label " << maxLabelIndex << std::endl;
                    }

                    exampleIterator.Next();
                    test_index++;
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
            auto model = map.GetModel();
            auto predictorNode = model.AddNode<nodes::ProtoNNPredictorNode>(map.GetOutputElements<double>(0), predictor);
            (void) model.AddNode<model::OutputNode<double>>(predictorNode->output);
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
