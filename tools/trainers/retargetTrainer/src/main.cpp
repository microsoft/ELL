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

// common
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"

// data
#include "Dataset.h"

// evaluators
#include "Evaluator.h"

// functions
#include "LogLoss.h"
#include "SquaredLoss.h"
#include "HingeLoss.h"
#include "SmoothHingeLoss.h"
#include "L2Regularizer.h"

// model
#include "DynamicMap.h"

// nodes
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "LinearPredictorNode.h"
#include "MatrixVectorProductNode.h"
#include "NeuralNetworkPredictorNode.h"

// predictors
#include "Normalizer.h"

// stl
#include <algorithm>
#include <iostream>

using namespace ell;

// predictor type
using PredictorType = predictors::LinearPredictor<double>;

template <typename ElementType>
model::DynamicMap AppendTrainedLinearPredictorToMap(const PredictorType& trainedPredictor, model::DynamicMap& map, size_t dimension)
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
bool RedirectNeuralNetworkOutputByLayer(model::DynamicMap& map, size_t numLayersFromEnd)
{
    bool found = false;
    auto nodes = (map.GetModel()).GetNodesByType<nodes::NeuralNetworkPredictorNode<ElementType>>();
    if (nodes.size() > 0)
    {
        auto predictor = nodes[0]->GetPredictor();
        predictor.RemoveLastLayers(numLayersFromEnd);
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<ElementType>>(predictor.GetInputShape());
        auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, predictor);

        map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });
        found = true;
    }

    return found;
}

bool RedirectModelOutputByNode(model::DynamicMap& map, const std::string& targetNodeId, size_t refineIterations)
{
    bool found = false;

    // Refine the model
    map.Refine(refineIterations);
    auto originalNode = map.GetModel().GetNode(model::Node::NodeId(targetNodeId));
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
    evaluator->Print(os);
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

template <typename LossFunctionType>
PredictorType RetargetModelUsingLinearPredictor(ParsedRetargetArguments& retargetArguments, data::AutoSupervisedDataset& dataset)
{
    trainers::SDCATrainerParameters trainerParameters{ retargetArguments.regularization, retargetArguments.desiredPrecision, retargetArguments.maxEpochs, retargetArguments.permute, retargetArguments.randomSeedString };

    auto trainer = trainers::SDCATrainer<LossFunctionType, functions::L2Regularizer>(LossFunctionType(), functions::L2Regularizer(), trainerParameters);
    if (retargetArguments.verbose) std::cout << "Created linear trainer ..." << std::endl;

    // create an evaluator
    evaluators::EvaluatorParameters evaluatorParameters{ 1, true };
    auto evaluator = common::MakeEvaluator<PredictorType>(dataset.GetAnyDataset(), evaluatorParameters, retargetArguments.lossFunctionArguments);

    // Train the predictor
    if (retargetArguments.verbose) std::cout << "Training ..." << std::endl;
    trainer.SetDataset(dataset.GetAnyDataset());
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

    return PredictorType(trainer.GetPredictor());
}

PredictorType RetargetModelUsingLinearPredictor(ParsedRetargetArguments& retargetArguments, data::AutoSupervisedDataset& dataset)
{
    using LossFunctionEnum = common::LossFunctionArguments::LossFunction;
    PredictorType trainedPredictor;
    switch (retargetArguments.lossFunctionArguments.lossFunction)
    {
    case LossFunctionEnum::squared:
        trainedPredictor = RetargetModelUsingLinearPredictor<functions::SquaredLoss>(retargetArguments, dataset);
        break;

    case LossFunctionEnum::log:
        trainedPredictor = RetargetModelUsingLinearPredictor<functions::LogLoss>(retargetArguments, dataset);
        break;

    case LossFunctionEnum::smoothHinge:
        trainedPredictor = RetargetModelUsingLinearPredictor<functions::SmoothHingeLoss>(retargetArguments, dataset);
        break;

    default:
        throw utilities::CommandLineParserErrorException("chosen loss function is not supported by this trainer");
    }
    return trainedPredictor;
}

std::vector<data::AutoSupervisedDataset> CreateDatasetsForOneVersusRest(data::AutoSupervisedMultiClassDataset& multiclassDataset)
{
    std::vector<data::AutoSupervisedDataset> datasets;

    // Get statistics for the multi-class dataset
    std::map<size_t, size_t> classCounts;
    for (size_t i = 0; i < multiclassDataset.NumExamples(); ++i)
    {
        const auto& example = multiclassDataset.GetExample(i);
        size_t classIndex = example.GetMetadata().classIndex;

        classCounts[classIndex] += 1;
    }

    // Create binary classification datasets for each class
    size_t numClasses = classCounts.size();
    size_t totalCount = multiclassDataset.NumExamples();
    datasets.resize(numClasses);
    for (size_t i = 0; i < numClasses; ++i)
    {
        // For any class x, create a binary classification dataset where Example is:
        //  weight = 1 / number of examples for x, or 1 / number of examples for all classes not x
        //  label = 1.0 for examples in x, or -1.0 for all examples not in x
        //  data = shared_ptr to existing data
        size_t positiveCount = classCounts[i];
        size_t negativeCount = (totalCount - classCounts[i]);
        double weightPositiveCase = 1.0 / (positiveCount ? positiveCount : 1.0);
        double weightNegativeCase = 1.0 / (negativeCount ? negativeCount : 1.0);
        datasets[i] = multiclassDataset.Transform<data::AutoSupervisedExample>([i, weightPositiveCase, weightNegativeCase] (const auto& example)
        {
            if (example.GetMetadata().classIndex == i)
            {
                // Positive case
                return data::AutoSupervisedExample(example.GetSharedDataVector(), data::WeightLabel{ weightPositiveCase, 1.0 });
            }
            else
            {
                // Negative case
                return data::AutoSupervisedExample(example.GetSharedDataVector(), data::WeightLabel{ weightNegativeCase, -1.0 });
            }
        });
    }

    return datasets;
}

template <typename ElementType>
model::DynamicMap GetMultiClassMapFromBinaryPredictors(std::vector<PredictorType>& binaryPredictors, model::DynamicMap& map)
{
    if (binaryPredictors.empty())
    {
        throw std::invalid_argument("binaryPredictors vector has no elements");
    }
    auto inputDimension = map.GetOutput(0).Size();
    for (auto& binaryPredictor : binaryPredictors)
    {
        binaryPredictor.Resize(inputDimension);
    }

    math::RowMatrix<ElementType> weights(binaryPredictors.size(), inputDimension);
    math::ColumnVector<ElementType> bias(binaryPredictors.size());

    // Set the weights matrix and bias from the predictors.
    // Each row in the weights is the learned weights from that predictor.
    // Each element in the bias is the learned bias from that predictor.
    for (size_t i = 0; i < binaryPredictors.size(); ++i)
    {
        auto predictorWeights = binaryPredictors[i].GetWeights();
        for (size_t j = 0; j < inputDimension; ++j)
        {
            weights(i, j) = predictorWeights[j];
        }
        bias[i] = binaryPredictors[i].GetBias();
    }

    model::Model& model = map.GetModel();
    auto mapOutput = map.GetOutputElements<ElementType>(0);
    auto matrixMultiplyNode = model.AddNode<nodes::MatrixVectorProductNode<ElementType, math::MatrixLayout::rowMajor>>(mapOutput, weights);
    auto biasNode = model.AddNode<nodes::ConstantNode<ElementType>>(bias.ToArray());
    auto addNode = model.AddNode<nodes::BinaryOperationNode<ElementType>>(matrixMultiplyNode->output, biasNode->output, emitters::BinaryOperationType::add);

    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(addNode->output);

    auto& output = outputNode->output;
    auto outputMap = model::DynamicMap(model, { { "input", map.GetInput() } }, { { "output", output } });

    return outputMap;
}

model::DynamicMap GetRetargetedModel(std::vector<PredictorType>& binaryPredictors, model::DynamicMap& map)
{
    model::DynamicMap result;
    // Create a new map with the output of the combined linear predictors appended.
    switch (map.GetOutputType())
    {
    case model::Port::PortType::smallReal:
    {
        result = GetMultiClassMapFromBinaryPredictors<float>(binaryPredictors, map);
        break;
    }
    case model::Port::PortType::real:
    {
        result = GetMultiClassMapFromBinaryPredictors<double>(binaryPredictors, map);
        break;
    }
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unexpected output type for model. Should be double or float.");
        break;
    };
    return result;
}

model::DynamicMap GetRetargetedModel(const PredictorType& trainedPredictor, model::DynamicMap& map)
{
    model::DynamicMap result;
    auto mappedDatasetDimension = map.GetOutput(0).Size();
    // Create a new map with the output of the linear predictor appended.
    switch (map.GetOutputType())
    {
    case model::Port::PortType::smallReal:
    {
        result = AppendTrainedLinearPredictorToMap<float>(trainedPredictor, map, mappedDatasetDimension);
        break;
    }
    case model::Port::PortType::real:
    {
        result = AppendTrainedLinearPredictorToMap<double>(trainedPredictor, map, mappedDatasetDimension);
        break;
    }
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unexpected output type for model. Should be double or float.");
        break;
    };

    return model::DynamicMap();
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
        if (retargetArguments.verbose) std::cout << "Loading model from " << retargetArguments.inputModelFilename << std::endl;
        auto map = common::LoadMap(retargetArguments.inputModelFilename);

        // Create a map by redirecting a layer or node to be output
        bool redirected = false;
        if (retargetArguments.removeLastLayers > 0)
        {
            if (map.GetOutputType() == model::Port::PortType::smallReal)
            {
                redirected = RedirectNeuralNetworkOutputByLayer<float>(map, retargetArguments.removeLastLayers);
                if (redirected && retargetArguments.verbose) std::cout << "Removed last " << retargetArguments.removeLastLayers << " layers from neural network" << std::endl;
            }
            else
            {
                redirected = RedirectNeuralNetworkOutputByLayer<double>(map, retargetArguments.removeLastLayers);
                if (redirected && retargetArguments.verbose) std::cout << "Removed last " << retargetArguments.removeLastLayers << " layers from neural network" << std::endl;
            }
        }
        else if (retargetArguments.targetNodeId.length() > 0)
        {
            redirected = RedirectModelOutputByNode(map, retargetArguments.targetNodeId, retargetArguments.refineIterations);
            if (redirected && retargetArguments.verbose) std::cout << "Redirected output for Node " << retargetArguments.targetNodeId << " from model" << std::endl;
        }
        else
        {
            std::cerr << "Error: Expected valid arguments for either --removeLastLayers or --targetNodeId" << std::endl;
            return 1;
        }

        if (!redirected)
        {
            std::cerr << "Could not splice model, exiting" << std::endl;
            return 1;
        }

        // load dataset and map the output
        if (retargetArguments.verbose) std::cout << "Loading data ..." << std::endl;
        model::DynamicMap result;
        if (retargetArguments.multiClass)
        {
            // This is a multi-class dataset
            auto stream = utilities::OpenIfstream(retargetArguments.inputDataFilename);
            auto multiclassDataset = common::GetMultiClassDataset(stream);
            // Obtain a new training dataset for the set of Linear Predictors by running the
            // multiclassDataset through the modified model
            auto dataset = common::TransformDataset(multiclassDataset, map);

            // Create binary classification datasets for each one versus rest (OVR) case
            auto datasets = CreateDatasetsForOneVersusRest(dataset);

            // Next, train a binary classifier for each case and combine into a
            // single model.
            std::vector<PredictorType> predictors(datasets.size());
            for (size_t i = 0; i < datasets.size(); ++i)
            {
                if (retargetArguments.verbose) std::cout << std::endl << "=== Training binary classifier for class " << i << " vs Rest ===" << std::endl;

                predictors[i] = RetargetModelUsingLinearPredictor(retargetArguments, datasets[i]);
            }

            // Save the newly spliced model
            result = GetRetargetedModel(predictors, map);
        }
        else
        {
            // This is a binary classification dataset
            auto stream = utilities::OpenIfstream(retargetArguments.inputDataFilename);
            auto binaryDataset = common::GetDataset(stream);
            // Obtain a new training dataset for the Linear Predictor by running the
            // binaryDataset through the modified model
            auto dataset = common::TransformDataset(binaryDataset, map);

            // Train a linear predictor whose input comes from the previous model
            auto predictor = RetargetModelUsingLinearPredictor(retargetArguments, dataset);

            // Save the newly spliced model
            result = GetRetargetedModel(predictor, map);
        }
        common::SaveMap(result, retargetArguments.outputModelFilename);
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
