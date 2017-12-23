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
#include "Map.h"
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

void CreateMap(predictors::ProtoNNPredictor& predictor, ell::model::Map& map)
{
    auto numFeatures = predictor.GetDimension();

    model::Model& model = map.GetModel();

    // add the input node.
    auto inputNode = model.AddNode<model::InputNode<double>>(numFeatures);

    // add the predictor node, taking input from the input node
    model::PortElements<double> inputElements(inputNode->output);
    auto predictorNode = model.AddNode<nodes::ProtoNNPredictorNode>(inputElements, predictor);

    // add an output node taking input from the predictor node.
    auto outputNode = model.AddNode<model::OutputNode<double>>(predictorNode->output);
    model::PortElements<double> outputElements(outputNode->output);

    // name the inputs and outputs to the map.
    map.AddInput("input", inputNode);
    map.AddOutput("output", outputElements);
}

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

        protoNNTrainerArguments.numFeatures = dimension;
        // create protonn trainer
        auto trainer = common::MakeProtoNNTrainer(protoNNTrainerArguments);

        // Train the predictor
        if (protoNNTrainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset(0, mappedDataset.NumExamples()));

        for (size_t i = 0; i < protoNNTrainerArguments.numIterations; i++)
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
            // Create a Map
            model::Map map;
            CreateMap(predictor, map);
            common::SaveMap(map, modelSaveArguments.outputModelFilename);
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
