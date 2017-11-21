////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (utilities)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RetargetArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"

// data
#include "Dataset.h"

// common
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"

// functions
#include "LogLoss.h"
#include "SquaredLoss.h"
#include "HingeLoss.h"
#include "SmoothHingeLoss.h"
#include "L2Regularizer.h"

// model
#include "DynamicMap.h"

// nodes
#include "LinearPredictorNode.h"
#include "NeuralNetworkPredictorNode.h"

// evaluators
#include "Evaluator.h"

// predictors
#include "Normalizer.h"

// stl
#include <iostream>

using namespace ell;

// predictor type
using PredictorType = predictors::LinearPredictor<double>;

template <typename ElementType>
model::DynamicMap AppendTrainedLinearPredictorToMap(const predictors::LinearPredictor<double>& trainedPredictor, model::DynamicMap& map, size_t dimension)
{
    predictors::LinearPredictor<ElementType> predictor(trainedPredictor);
    predictor.Resize(dimension);

    model::Model& model = map.GetModel();
    auto mapOutput = map.GetOutputElements<ElementType>(0);
    auto predictorNode = model.AddNode<nodes::LinearPredictorNode<ElementType>>(mapOutput, predictor);
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(predictorNode->output);

    auto& output = outputNode->output;
    auto outputMap = model::DynamicMap(map.GetModel(), { { "input", map.GetInput() } }, { { "output", output } });

    return outputMap;
}

template <typename ElementType>
bool CutNeuralNetworkUsingLayers(model::DynamicMap& map, ParsedRetargetArguments& retargetArguments)
{
    bool found = false;
    auto nodes = (map.GetModel()).GetNodesByType<nodes::NeuralNetworkPredictorNode<ElementType>>();
    if (nodes.size() > 0)
    {
        auto predictor = nodes[0]->GetPredictor();
        predictor.RemoveLastLayers(retargetArguments.removeLastLayers);
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<ElementType>>(predictor.GetInputShape());
        auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, predictor);

        map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });
        found = true;
    }

    if (found && retargetArguments.verbose)
    {
        std::cout << "Removed last " << retargetArguments.removeLastLayers << " layers from neural network" << std::endl;
    }

    return found;
}

bool CutNeuralNetworkUsingNode(model::DynamicMap& map, ParsedRetargetArguments& retargetArguments)
{
    bool found = false;

    // Refine the model
    map.Refine(retargetArguments.refineIterations);
    auto originalNode = map.GetModel().GetNode(model::Node::NodeId(retargetArguments.targetNodeId));
    if (originalNode)
    {
        // Find the node
        model::TransformContext context;
        model::ModelTransformer transformer;
        auto model = transformer.CopyModel(map.GetModel(), context);
        auto input = transformer.GetCorrespondingInputNode(map.GetInput());
        auto output = transformer.GetCorrespondingOutputs(*originalNode->GetOutputPort(0));

        map = model::DynamicMap(model, {{ "input", input }}, {{ "output", output }});
        found = true;
    }

    if (found && retargetArguments.verbose)
    {
        std::cout << "Cutting Neural Network at output of node " << retargetArguments.targetNodeId << std::endl;
    }
    return found;
}

void PrintSDCAPredictorInfoHeader(std::ostream& os)
{
    os << "\tPrimal Objective";
    os << "\tDual Objective";
    os << "\tDuality gap";
    os << std::endl;
}

void PrintSDCAPredictorInfoValues(const trainers::SDCAPredictorInfo& info, std::ostream& os)
{
    auto originalPrecision = os.precision(6);
    auto originalFlags = os.setf(std::ios::fixed);

    os << "\t" << info.primalObjective;
    os << "\t\t" << info.dualObjective;
    os << "\t" << std::abs(info.primalObjective - info.dualObjective) << std::endl;

    os.setf(originalFlags);
    os.precision(originalPrecision);
}

void PrintEvaluation(double dualityGap, double desiredPrecision, evaluators::IEvaluator<PredictorType>* evaluator,  std::ostream& os)
{
    // Print evaluation of training
    os << "Final duality Gap: " << dualityGap << std::endl << std::endl;
    evaluator->Print(std::cout);
    os << std::endl << std::endl;
    if (dualityGap < desiredPrecision)
    {
        os << "Training completed successfully." << std::endl;
    }
    else
    {
        os << "Warning: After maxEpochs, training has not converged to desired duality gap." << std::endl;
    }
}

void SaveRetargetedModel(const predictors::LinearPredictor<double>& trainedPredictor, model::DynamicMap& map,  const std::string& filename)
{
    auto mappedDatasetDimension = map.GetOutput(0).Size();
    // Create a new map with the linear predictor appended.
    switch (map.GetOutputType())
    {
    case model::Port::PortType::smallReal:
        {
            auto outputMap = AppendTrainedLinearPredictorToMap<float>(trainedPredictor, map, mappedDatasetDimension);
            common::SaveMap(outputMap, filename);
        }
        break;
    case model::Port::PortType::real:
        {
            auto outputMap = AppendTrainedLinearPredictorToMap<double>(trainedPredictor, map, mappedDatasetDimension);
            common::SaveMap(outputMap, filename);
        }
        break;
    default:
        std::cerr << "Unexpected output type for model. Should be double or float." << std::endl;
        break;
    };
}

template <typename LossFunctionType>
void RetargetNetworkUsingLinearPredictor(ParsedRetargetArguments& retargetArguments, data::AutoSupervisedDataset& mappedDataset, model::DynamicMap& map)
{
    trainers::SDCATrainerParameters trainerParameters{ retargetArguments.regularization, retargetArguments.desiredPrecision, retargetArguments.maxEpochs, retargetArguments.permute, retargetArguments.randomSeedString };

    auto trainer = trainers::SDCATrainer<LossFunctionType, functions::L2Regularizer>(LossFunctionType(), functions::L2Regularizer(), trainerParameters);
    if (retargetArguments.verbose) std::cout << "Created linear trainer ..." << std::endl;

    // create an evaluator
    evaluators::EvaluatorParameters evaluatorParameters{1, true};
    auto evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorParameters, retargetArguments.lossFunctionArguments);

    // Train the predictor
    if (retargetArguments.verbose) std::cout << "Training ..." << std::endl;
    trainer.SetDataset(mappedDataset.GetAnyDataset());
    size_t epoch = 0;
    double dualityGap = std::numeric_limits<double>::max();

    if (retargetArguments.verbose) PrintSDCAPredictorInfoHeader(std::cout);
    while ((++epoch <= retargetArguments.maxEpochs) && (dualityGap > retargetArguments.desiredPrecision))
    {
        trainer.Update();
        auto info = trainer.GetPredictorInfo();
        dualityGap = std::abs(info.primalObjective - info.dualObjective);
        if (retargetArguments.verbose) PrintSDCAPredictorInfoValues(trainer.GetPredictorInfo(), std::cout);
    }

    // Print evaluation of training
    evaluator->Evaluate(trainer.GetPredictor());
    PrintEvaluation(dualityGap, retargetArguments.desiredPrecision, evaluator.get(), std::cout);

    // Save new model which has headless neural network followed by the linear predictor
    if (retargetArguments.outputModelFilename != "")
    {
        if (retargetArguments.verbose) std::cout << "Saving retargeted model to " << retargetArguments.outputModelFilename << std::endl;
        SaveRetargetedModel(trainer.GetPredictor(), map, retargetArguments.outputModelFilename);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedRetargetArguments retargetArguments;
        commandLineParser.AddOptionSet(retargetArguments);

        // parse command line
        commandLineParser.Parse();
        if (retargetArguments.verbose) std::cout << commandLineParser.GetCurrentValuesString() << std::endl;

        // load map
        if (retargetArguments.verbose) std::cout << "Loading model from " << retargetArguments.neuralNetworkFilename << std::endl;
        auto map = common::LoadMap(retargetArguments.neuralNetworkFilename);

        // Cut the map
        bool cut = false;
        if (retargetArguments.removeLastLayers > 0)
        {
            if (map.GetOutputType() == model::Port::PortType::smallReal)
            {
                cut = CutNeuralNetworkUsingLayers<float>(map, retargetArguments);
            }
            else
            {
                cut = CutNeuralNetworkUsingLayers<double>(map, retargetArguments);
            }
        }
        else if (retargetArguments.targetNodeId.length() > 0)
        {
            cut = CutNeuralNetworkUsingNode(map, retargetArguments);
        }
        else
        {
            std::cerr << "Error: Expected valid arguments for either --removeLastLayers or --targetNodeId" << std::endl;
            return 1;
        }

        if (!cut)
        {
            std::cerr << "Could not cut neural network, exiting" << std::endl;
            return 1;
        }

        // load dataset and map the output
        if (retargetArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto stream = utilities::OpenIfstream(retargetArguments.inputDataFilename);
        auto mappedDataset = common::GetMappedDataset(stream, map);

        // Train a linear predictor and splice it onto the previously cut neural network
        using LossFunctionEnum = common::LossFunctionArguments::LossFunction;
        switch (retargetArguments.lossFunctionArguments.lossFunction)
        {
        case LossFunctionEnum::squared:
            RetargetNetworkUsingLinearPredictor<functions::SquaredLoss>(retargetArguments, mappedDataset, map);
            break;

        case LossFunctionEnum::log:
            RetargetNetworkUsingLinearPredictor<functions::LogLoss>(retargetArguments, mappedDataset, map);
            break;

        case LossFunctionEnum::smoothHinge:
            break; RetargetNetworkUsingLinearPredictor<functions::SmoothHingeLoss>(retargetArguments, mappedDataset, map);

        default:
            throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
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
