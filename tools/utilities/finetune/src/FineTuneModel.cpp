////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FineTuneModel.cpp (finetune)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FineTuneModel.h"
#include "DataUtils.h"
#include "FineTuneArguments.h"
#include "ModelOutputDataCache.h"
#include "ModelUtils.h"
#include "OptimizationUtils.h"
#include "Report.h"
#include "TransformData.h"

#include <common/include/LoadModel.h>

#include <model/include/Map.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <nodes/include/ConvolutionalLayerNode.h>
#include <nodes/include/FullyConnectedLayerNode.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/MillisecondTimer.h>

#include <algorithm>
#include <string>
#include <utility>

namespace ell
{
using namespace ell::model;

// Prototypes
FineTuneNodeAction GetNodeAction(const Node& node, const FineTuneArguments& args, bool didModifyAnyNodes);
const FineTuneOptimizationParameters& GetParametersForNodeAction(const FineTuneProblemParameters& parameters, FineTuneNodeAction action);

FineTuningLayerResult FineTuneLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);
FineTuningLayerResult SparsifyLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);

FineTuningLayerResult RetrainLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);

template <typename ElementType>
FineTuningLayerResult RetrainLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);

template <typename ElementType>
FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);

template <typename ElementType>
FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache);

template <typename ElementType>
const OutputPort<ElementType>& GetFeaturesPort(const UnlabeledDataContainer& imageFeatures, const OutputPort<ElementType>& rawFeatureOutput, bool normalize);

template <typename ElementType>
DataStatistics GetWeightsStatistics(const WeightsAndBias<ElementType>& weightsAndBias);

template <typename ElementType>
DataStatistics GetWeightsStatistics(const ell::math::RowMatrix<ElementType>& weights);

template <typename ElementType>
DataStatistics GetWeightsStatistics(const typename predictors::neural::ConvolutionalLayer<ElementType>::TensorType& weights);

template <typename DatasetType>
void ConvertDatasetImages(DatasetType& dataset, const FineTuneArguments& args);

// Implementation
#define ADD_TO_STRING_ENTRY(NAMESPACE, OPERATOR) \
    case NAMESPACE::OPERATOR:                    \
        return #OPERATOR;
#define BEGIN_FROM_STRING if (false)
#define ADD_FROM_STRING_ENTRY(NAMESPACE, OPERATOR) else if (name == #OPERATOR) return NAMESPACE::OPERATOR

std::string ToString(FineTuneNodeAction action)
{
    switch (action)
    {
        ADD_TO_STRING_ENTRY(FineTuneNodeAction, copy);
        ADD_TO_STRING_ENTRY(FineTuneNodeAction, finetune);
        ADD_TO_STRING_ENTRY(FineTuneNodeAction, sparsify);
        ADD_TO_STRING_ENTRY(FineTuneNodeAction, reoptimize);
        ADD_TO_STRING_ENTRY(FineTuneNodeAction, none);
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown node action type");
    }
}

template <typename ElementType>
void VerifyConvolutionalWeightsMatrix(const math::RowMatrix<ElementType>& weights, int filterSize, int numInputChannels, int numOutputChannels, bool isSpatialConvolution)
{
    // matrix is f x (k*k*d)
    if (static_cast<int>(weights.NumRows()) != numOutputChannels)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Convolution weights matrix: wrong number of rows");
    }
    if (static_cast<int>(weights.NumColumns()) != filterSize * filterSize * (isSpatialConvolution ? 1 : numInputChannels))
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Convolution weights matrix: wrong number of columns");
    }
}

void VerifySubmodel(const OutputPortBase& submodelInput, const OutputPortBase& submodelOutput)
{
    // Make sure submodelOutput is dependent on submodelInput
    bool ok = false;
    submodelOutput.GetNode()->GetModel()->VisitSubmodel(&submodelOutput, [&ok, &submodelInput](const Node& node) {
        for (auto port : node.GetInputPorts())
        {
            if (&port->GetReferencedPort() == &submodelInput)
            {
                ok = true;
            }
        }
    });
    if (!ok)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid submodel, port " + submodelOutput.GetFullName() + " doesn't depend on " + submodelInput.GetFullName());
    }
}

template <typename ElementType, typename ParametersType>
struct NodeTypeFromParametersType
{};

template <typename ElementType>
struct NodeTypeFromParametersType<ElementType, FullyConnectedParameters>
{
    using NodeType = ell::nodes::FullyConnectedLayerNode<ElementType>;
};

template <typename ElementType>
struct NodeTypeFromParametersType<ElementType, ConvolutionalParameters>
{
    using NodeType = ell::nodes::ConvolutionalLayerNode<ElementType>;
};

template <typename ElementType, typename LayerParametersType>
const typename NodeTypeFromParametersType<ElementType, LayerParametersType>::NodeType* GetNearestFineTunableNodeForParams(const ell::model::OutputPortBase& output)
{
    if constexpr (std::is_same_v<LayerParametersType, ConvolutionalParameters>)
    {
        return GetNearestConvolutionalLayerNode<ElementType>(output);
    }
    else if constexpr (std::is_same_v<LayerParametersType, FullyConnectedParameters>)
    {
        return GetNearestFullyConnectedLayerNode<ElementType>(output);
    }
    else
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
    }
}

BinaryLabelDataContainer GetBinaryTrainingDataset(const FineTuneArguments& args)
{
    return LoadBinaryLabelDataContainer(args.trainDataArguments.inputDataFilename);
}

MultiClassDataContainer GetMultiClassTrainingDataset(const FineTuneArguments& args)
{
    auto datasetFilename = args.trainDataArguments.inputDataFilename;
    if (datasetFilename.empty())
    {
        return {};
    }

    auto dataFormat = args.dataFormat;
    auto maxRows = args.maxTrainingRows;
    auto dataset = LoadMultiClassDataContainer(datasetFilename, dataFormat, maxRows);
    ConvertDatasetImages(dataset, args);
    return dataset;
}

BinaryLabelDataContainer GetBinaryTestDataset(const FineTuneArguments& args)
{
    auto datasetFilename = args.testDataArguments.inputDataFilename;
    if (datasetFilename.empty())
    {
        return {};
    }

    return LoadBinaryLabelDataContainer(args.testDataArguments.inputDataFilename);
}

MultiClassDataContainer GetMultiClassTestDataset(const FineTuneArguments& args)
{
    auto datasetFilename = args.testDataArguments.inputDataFilename;
    if (datasetFilename.empty())
    {
        return {};
    }

    auto dataFormat = args.dataFormat;
    auto maxRows = args.maxTestingRows;
    auto dataset = LoadMultiClassDataContainer(datasetFilename, dataFormat, maxRows);
    ConvertDatasetImages(dataset, args);
    return dataset;
}

template <typename DatasetType>
void ConvertDatasetImages(DatasetType& dataset, const FineTuneArguments& args)
{
    // Convert image data
    const bool adjustMean = false;
    const bool swapChannels = false;

    for (auto& row : dataset)
    {
        if (adjustMean)
        {
            row.input += 128;
        }

        if (swapChannels)
        {
            // RGB <--> BGR
            auto size = row.input.Size();
            for (size_t i = 0; i < size; i += 3)
            {
                std::swap(row.input[i], row.input[i + 2]);
            }
        }
    }
}

FineTuningResult FineTuneNodesInSubmodel(const Submodel& submodel,
                                         MultiClassDataContainer& trainingData,
                                         const FineTuneArguments& args,
                                         std::function<void(const FineTuningLayerResult&)> layerCallback)
{
    TransformContext context;
    ModelTransformer transformer;

    std::chrono::milliseconds::rep dataTransformTime = 0;
    std::chrono::milliseconds::rep optimizationTime = 0;
    std::vector<FineTuningLayerResult> layerResults;

    ModelOutputDataCache dataCache(args.maxCacheEntries);

    bool didModifyAnyNodes = false;
    auto problemParams = args.GetFineTuneProblemParameters();
    auto resultSubmodel = transformer.TransformSubmodel(submodel, context, [&problemParams, &didModifyAnyNodes, &layerResults, &dataTransformTime, &optimizationTime, &dataCache, &trainingData, &args, &layerCallback](const Node& node, ModelTransformer& transformer) {
        FineTuningLayerResult retrainingResult;
        auto action = GetNodeAction(node, args, didModifyAnyNodes);

        if (action != FineTuneNodeAction::none)
        {
            using namespace logging;
            Log() << "Action for node " << node.GetId() << ": " << ToString(action) << std::endl;
        }

        switch (action)
        {
        case FineTuneNodeAction::finetune:
            retrainingResult = FineTuneLayer(transformer, node, trainingData, problemParams, dataCache);
            didModifyAnyNodes = true;
            break;
        case FineTuneNodeAction::sparsify:
            retrainingResult = SparsifyLayer(transformer, node, trainingData, problemParams, dataCache);
            didModifyAnyNodes = true;
            break;
        default:
            transformer.CopyNode(node);
            return;
        }
        dataTransformTime += retrainingResult.dataTransformTime;
        optimizationTime += retrainingResult.optimizationTime;
        if (layerCallback)
        {
            layerCallback(retrainingResult);
        }
        layerResults.push_back(retrainingResult);
    });

    return { layerResults, resultSubmodel, dataTransformTime, optimizationTime };
}

TargetNodeType GetConvNodeTargetType(const Node& node)
{
    auto convType = GetConvolutionalNodeType(&node);
    switch (convType)
    {
    case ConvolutionalNodeType::full:
        return TargetNodeType::fullConvolution;
    case ConvolutionalNodeType::pointwise:
        return TargetNodeType::pointwiseConvolution;
    case ConvolutionalNodeType::spatial:
        return TargetNodeType::spatialConvolution;
    }
    return TargetNodeType::none;
}

TargetNodeType GetNodeTargetType(const Node& node)
{
    if (IsConvolutionalLayerNode(&node))
    {
        return GetConvNodeTargetType(node);
    }
    else if (IsFullyConnectedLayerNode(&node))
    {
        return TargetNodeType::fullyConnected;
    }
    return TargetNodeType::none;
}

FineTuneNodeAction GetNodeAction(const Node& node, const FineTuneArguments& args, bool didModifyAnyNodes)
{
    if (!IsFullyConnectedLayerNode(&node) && !IsConvolutionalLayerNode(&node))
    {
        return FineTuneNodeAction::none;
    }

    if (args.SkipNode(node.GetId().ToString()))
    {
        using namespace logging;
        Log() << "Skipping node " << node.GetId() << EOL;
        return didModifyAnyNodes ? FineTuneNodeAction::finetune : FineTuneNodeAction::copy;
    }

    if ((args.fineTuneFullyConnectedNodes && IsFullyConnectedLayerNode(&node)) ||
        (args.fineTuneConvolutionalNodes && IsConvolutionalLayerNode(&node)))
    {
        auto nodeTargetType = GetNodeTargetType(node);
        auto sparsifyTargets = args.sparsifyTargets;
        if (sparsifyTargets & nodeTargetType)
        {
            return FineTuneNodeAction::sparsify;
        }
        else
        {
            return FineTuneNodeAction::finetune;
        }
    }

    return FineTuneNodeAction::none;
}

bool ShouldSparsifyNode(const Node& node, const FineTuneArguments& args)
{
    auto nodeTargetType = GetNodeTargetType(node);
    if (nodeTargetType == TargetNodeType::none)
    {
        return false;
    }
    auto sparsifyTargets = args.sparsifyTargets;
    return sparsifyTargets & nodeTargetType;
}

const FineTuneOptimizationParameters& GetParametersForNodeAction(const FineTuneProblemParameters& parameters, FineTuneNodeAction action)
{
    switch (action)
    {
    case FineTuneNodeAction::finetune:
        return parameters.fineTuneParameters;
    case FineTuneNodeAction::sparsify:
        return parameters.sparsifyParameters;
    case FineTuneNodeAction::reoptimize:
        return parameters.reoptimizeParameters;
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unsupported action type");
    }
}

FineTuningLayerResult FineTuneLayer(ModelTransformer& transformer,
                                    const Node& node,
                                    MultiClassDataContainer& trainingData,
                                    const FineTuneProblemParameters& optimizerParameters,
                                    ModelOutputDataCache& dataCache)
{
    return RetrainLayer(transformer, node, FineTuneNodeAction::finetune, trainingData, optimizerParameters, dataCache);
}

FineTuningLayerResult SparsifyLayer(ModelTransformer& transformer,
                                    const Node& node,
                                    MultiClassDataContainer& trainingData,
                                    const FineTuneProblemParameters& optimizerParameters,
                                    ModelOutputDataCache& dataCache)
{
    return RetrainLayer(transformer, node, FineTuneNodeAction::sparsify, trainingData, optimizerParameters, dataCache);
}

FineTuningLayerResult RetrainLayer(ModelTransformer& transformer,
                                   const Node& node,
                                   FineTuneNodeAction action,
                                   MultiClassDataContainer& trainingData,
                                   const FineTuneProblemParameters& optimizerParameters,
                                   ModelOutputDataCache& dataCache)
{
    if (node.NumOutputPorts() != 1)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unsupported node type");
    }

    switch (node.GetOutputPort(0)->GetType())
    {
    case model::Port::PortType::smallReal:
        return RetrainLayer<float>(transformer, node, action, trainingData, optimizerParameters, dataCache);
        break;

    case model::Port::PortType::real:
        return RetrainLayer<double>(transformer, node, action, trainingData, optimizerParameters, dataCache);
        break;

    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unsupported port type");
    }
}

template <typename ElementType>
FineTuningLayerResult RetrainLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache)
{
    if (IsFullyConnectedLayerNode(&node)) // TODO: replace with submodel-matcher
    {
        return RetrainFullyConnectedLayer<ElementType>(transformer, node, action, trainingData, optimizerParameters, dataCache);
    }
    else if (IsConvolutionalLayerNode(&node)) // TODO: replace with submodel-matcher
    {
        return RetrainConvolutionalLayer<ElementType>(transformer, node, action, trainingData, optimizerParameters, dataCache);
    }
    else
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempting to retrain an unsupported layer type");
    }
}

template <typename ElementType>
FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache)
{
    auto fcNode = dynamic_cast<const nodes::FullyConnectedLayerNode<ElementType>*>(&node);
    const auto& fcOutput = fcNode->output;

    const auto& destination = transformer.GetCorrespondingOutputs(fcNode->input.GetReferencedPort());
    FullyConnectedParameters fcParams;
    auto fineTunedOutput = ApproximateSubmodelWithNewLayer(trainingData, fcParams, fcOutput, destination, action, optimizerParameters, dataCache);
    transformer.MapNodeOutput(fcOutput, *fineTunedOutput.fineTunedOutput);
    return fineTunedOutput;
}

template <typename ElementType>
FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, FineTuneNodeAction action, MultiClassDataContainer& trainingData, const FineTuneProblemParameters& optimizerParameters, ModelOutputDataCache& dataCache)
{
    // TODO: get input of submodel to retrain
    auto convolutionType = GetConvolutionalNodeType(&node);
    auto convNode = dynamic_cast<const nodes::ConvolutionalLayerNode<ElementType>*>(&node);
    const auto& convOutput = convNode->output;
    const auto& convLayer = convNode->GetLayer();
    const auto filterSize = static_cast<int>(convLayer.GetConvolutionalParameters().receptiveField);
    const auto stride = static_cast<int>(convLayer.GetConvolutionalParameters().stride);
    const auto inputPadding = static_cast<int>(convLayer.GetLayerParameters().inputPaddingParameters.paddingSize);
    const auto outputPadding = static_cast<int>(convLayer.GetLayerParameters().outputPaddingParameters.paddingSize);
    const bool isSpatialConvolution = convolutionType == ConvolutionalNodeType::spatial;

    const auto& destination = transformer.GetCorrespondingOutputs(convNode->input.GetReferencedPort());
    ConvolutionalParameters convParams{ filterSize, stride, isSpatialConvolution, inputPadding, outputPadding };
    auto fineTunedOutput = ApproximateSubmodelWithNewLayer(trainingData, convParams, convOutput, destination, action, optimizerParameters, dataCache);
    transformer.MapNodeOutput(convOutput, *fineTunedOutput.fineTunedOutput);
    return fineTunedOutput;
}

// TODO: rename this function to imply we're running an optimization
template <typename ElementType, typename LayerParametersType>
FineTuningLayerResult ApproximateSubmodelWithNewLayer(const MultiClassDataContainer& imageData,
                                                      const LayerParametersType& layerParams,
                                                      const OutputPort<ElementType>& submodelOutput,
                                                      const OutputPort<ElementType>& destination,
                                                      FineTuneNodeAction action,
                                                      const FineTuneProblemParameters& problemParameters,
                                                      ModelOutputDataCache& dataCache)
{
    using namespace logging;

    // Gets the nearest convolutional or fully-connected node (probaby the one with submodelOutput as its output)
    auto node = GetNearestFineTunableNodeForParams<ElementType, LayerParametersType>(submodelOutput);
    if (node == nullptr)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can't find original layer being fine-tuned");
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    Log() << "[" << std::put_time(std::localtime(&now), "%F %T") << "] Replacing node " << node->GetId() << " with fine-tuned version" << EOL;
    Log() << "Action: " << ToString(action) << std::endl;

    // Create a dataset from features -> labels
    utilities::MillisecondTimer dataTransformTimer;
    auto retrainingDataset = GetFineTuningDataset(imageData, layerParams, submodelOutput, destination, problemParameters.normalizeInputs, problemParameters.normalizeOutputs, dataCache);
    dataTransformTimer.Stop();
    auto dataTransformTime = dataTransformTimer.Elapsed();

    bool isSpatialConvolution = IsConvolutionalLayerNode(node) && GetConvolutionalNodeType(node) == ConvolutionalNodeType::spatial;

    // Run SDCA to find weights that map data1->data2
    utilities::MillisecondTimer optTimer;
    auto optimizerParameters = GetParametersForNodeAction(problemParameters, action);
    auto solution = TrainVectorPredictor(retrainingDataset.dataset, optimizerParameters, isSpatialConvolution);
    if (action == FineTuneNodeAction::sparsify && optimizerParameters.reoptimizeSparseWeights)
    {
        // TODO: record info about both phases (sparsify and reoptimize)
        auto reoptimizeParameters = GetParametersForNodeAction(problemParameters, FineTuneNodeAction::reoptimize);
        solution = ReoptimizeSparsePredictor(solution, retrainingDataset.dataset, reoptimizeParameters, isSpatialConvolution);
    }

    optTimer.Stop();
    auto optimizationTime = optTimer.Elapsed();

    Log() << "Optimization done: duality gap = " << solution.info.info.DualityGap() << std::endl;

    FineTuningStats stats;
    stats.originalActivationStatistics = retrainingDataset.labelStats;
    stats.originalWeightsStatistics = retrainingDataset.labelStats;
    stats.originalWeightsStatistics = GetWeightsStatistics<ElementType>(node->GetLayer().GetWeights());

    // Sparsity-inducing optimization failed, re-run optimizer with just L2 (a 'finetune' action)
    if (solution.info.info.DualityGap() > optimizerParameters.requiredPrecision && optimizerParameters.requiredPrecision != 0)
    {
        Log() << "Optimization failed: duality gap = " << solution.info.info.DualityGap() << std::endl;

        // record info about the failed attempt's solution (weights stats, solution.info.info, solution.l2Regularization, and solution.l1Regularization)
        auto failedWeightsAndBias = GetWeightsAndBias<ElementType>(solution.predictor);
        stats.failedWeightsStatistics = GetWeightsStatistics(failedWeightsAndBias);

        auto fineTuneOnlyParameters = optimizerParameters;
        fineTuneOnlyParameters.sparsityTarget = 0; // "none"
        fineTuneOnlyParameters.l1Regularization = 0; // "none"
        utilities::MillisecondTimer optTimer;
        solution = TrainVectorPredictor(std::move(retrainingDataset.dataset), optimizerParameters, isSpatialConvolution);
        optTimer.Stop();
        optimizationTime += optTimer.Elapsed();

        // if the L2-only optimization also failed to converge, punt and return the original layer
        if (solution.info.info.DualityGap() > optimizerParameters.requiredPrecision && optimizerParameters.requiredPrecision != 0)
        {
            // Get original weights statistics
            stats.finalWeightsStatistics = GetWeightsStatistics<ElementType>(node->GetLayer().GetWeights());

            // Copy original node
            ModelTransformer transformer;
            TransformContext context;
            auto newSubmodel = transformer.CopySubmodelOnto(Submodel{ std::vector<const InputPortBase*>{ &node->input }, std::vector<const OutputPortBase*>{ &submodelOutput } },
                                                            std::vector<const OutputPortBase*>{ &destination },
                                                            context);
            return FineTuningLayerResult{ false,
                                          &submodelOutput,
                                          newSubmodel.GetOutputs()[0],
                                          solution.info,
                                          stats,
                                          dataTransformTime,
                                          optimizationTime };
        }
    }

    auto weightsAndBias = GetWeightsAndBias<ElementType>(solution.predictor);
    if constexpr (std::is_same_v<LayerParametersType, ConvolutionalParameters>)
    {
        // Sanity check for convolutional nodes -- destination is where we're going to graft the new submodel, so it must have the same number of channels as the input to the convolution
        const auto numChannels = destination.GetMemoryLayout().GetActiveSize()[2];
        const auto numFilters = submodelOutput.GetMemoryLayout().GetActiveSize()[2];
        VerifyConvolutionalWeightsMatrix(weightsAndBias.weights, layerParams.filterSize, numChannels, numFilters, isSpatialConvolution);
    }

    // Add retrained convolutional and bias nodes
    const auto& rawFineTunedOutput = AppendFineTunedNodes(*retrainingDataset.normalizedFeaturesOutput, layerParams, weightsAndBias);
    auto output = std::cref(rawFineTunedOutput);
    if (problemParameters.normalizeOutputs)
    {
        // We need to normalize the new output to match original output
        // which means we need to run data through the model and get the stats at rawFineTunedOutput
        // then change it to match the original output's stats

        Submodel rawFineTunedDataSubmodel{ { static_cast<const OutputPortBase*>(&rawFineTunedOutput) } };
        auto imageFeatures = GetDatasetInputs(imageData);
        auto rawFineTunedOutputData = TransformDataWithSubmodel(imageFeatures, rawFineTunedDataSubmodel, dataCache, true);
        const int outputChannelDim = 2;
        stats.rawFineTunedActivationStatistics = GetDataStatistics(rawFineTunedOutputData, rawFineTunedOutput.GetMemoryLayout(), outputChannelDim);
        output = Unnormalize(rawFineTunedOutput, retrainingDataset.labelStats, stats.rawFineTunedActivationStatistics.value());
    }

    stats.finalWeightsStatistics = GetWeightsStatistics(weightsAndBias);
    {
        Submodel resultSubmodel{ { static_cast<const OutputPortBase*>(&(output.get())) } };
        auto imageFeatures = GetDatasetInputs(imageData);
        auto fineTunedOutput = TransformDataWithSubmodel(imageFeatures, resultSubmodel, dataCache, true);
        stats.fineTunedActivationStatistics = GetDataStatistics(fineTunedOutput);
    }

    return FineTuningLayerResult{ true,
                                  &submodelOutput,
                                  &(output.get()),
                                  solution.info,
                                  stats,
                                  dataTransformTime,
                                  optimizationTime };
}

template <typename ElementType>
FineTuningDataset<ElementType> GetFineTuningDataset(const MultiClassDataContainer& imageData,
                                                    const FullyConnectedParameters& fcParams,
                                                    const OutputPort<ElementType>& fullyConnectedOutput,
                                                    const OutputPort<ElementType>& destination,
                                                    bool normalizeInputs,
                                                    bool normalizeOutputs,
                                                    ModelOutputDataCache& dataCache)
{
    auto imageFeatures = GetDatasetInputs(imageData);
    const auto& featuresOutput = GetFeaturesPort(imageFeatures, destination, normalizeInputs);

    auto trainingFeatures = TransformDataWithModel(imageFeatures, featuresOutput);

    auto rawLabels = TransformDataWithModel(imageFeatures, fullyConnectedOutput);
    auto labelStats = GetDataStatistics(rawLabels);
    UnlabeledDataContainer trainingLabels = normalizeOutputs ? GetNormalizedData(rawLabels, labelStats) : rawLabels;

    // Create a dataset from features -> labels
    auto retrainingDataset = CreateVectorLabelDataContainer(trainingFeatures, trainingLabels);
    return { retrainingDataset, labelStats, &featuresOutput };
}

// TODO: Rename this function to make it clear we're adding nodes to the model (if normalizeInputs is true)
// --- maybe have calling function add the normalization and then call this with its output
template <typename ElementType>
FineTuningDataset<ElementType> GetFineTuningDataset(const MultiClassDataContainer& imageData,
                                                    const ConvolutionalParameters& convParams,
                                                    const OutputPort<ElementType>& convolutionOutput,
                                                    const OutputPort<ElementType>& destination,
                                                    bool normalizeInputs,
                                                    bool normalizeOutputs,
                                                    ModelOutputDataCache& dataCache)
{
    // TODO: fix this to work with depthwise-separable convolutions
    const auto numRows = destination.GetMemoryLayout().GetExtent()[0];
    const auto numColumns = destination.GetMemoryLayout().GetExtent()[1];
    const auto numChannels = destination.GetMemoryLayout().GetExtent()[2];
    const auto numOutputRows = convolutionOutput.GetMemoryLayout().GetActiveSize()[0];
    const auto numOutputColumns = convolutionOutput.GetMemoryLayout().GetActiveSize()[1];
    const int outputChannelDim = 2;
    const auto numFilters = convolutionOutput.GetMemoryLayout().GetActiveSize()[outputChannelDim];

    auto imageFeatures = GetDatasetInputs(imageData);
    const auto& featuresOutput = GetFeaturesPort(imageFeatures, destination, normalizeInputs);

    Submodel transformSubmodel1{ { &featuresOutput } };
    auto imageFeatureData = TransformDataWithSubmodel(imageFeatures, transformSubmodel1, dataCache, true); // acutally should be false

    Submodel transformSubmodel2{ { &convolutionOutput } };
    auto rawLabels = TransformDataWithSubmodel(imageFeatures, transformSubmodel2, dataCache, true);

    auto rawLabelStats = GetDataStatistics(rawLabels, convolutionOutput.GetMemoryLayout(), outputChannelDim);
    UnlabeledDataContainer imageLabelData = normalizeOutputs ? GetNormalizedData(rawLabels, rawLabelStats, convolutionOutput.GetMemoryLayout(), outputChannelDim) : rawLabels;
    UnlabeledDataContainer trainingFeatures = GetUnrolledImageDataset(imageFeatureData, numRows, numColumns, numChannels, convParams.filterSize, convParams.stride);
    UnlabeledDataContainer trainingLabels = GetImageMatrixDataset(imageLabelData, numOutputRows, numOutputColumns, numFilters);

    // Create a dataset from features -> labels
    auto retrainingDataset = CreateVectorLabelDataContainer(trainingFeatures, trainingLabels);
    return { retrainingDataset, rawLabelStats, &featuresOutput };
}

template <typename ElementType>
const OutputPort<ElementType>& GetFeaturesPort(const UnlabeledDataContainer& imageFeatures, const OutputPort<ElementType>& rawFeatureOutput, bool normalize)
{
    if (normalize)
    {
        auto rawFeatures = TransformDataWithModel(imageFeatures, rawFeatureOutput);
        auto featureStats = GetDataStatistics(rawFeatures);
        return Normalize(rawFeatureOutput, featureStats);
    }
    else
    {
        // no normalization
        return rawFeatureOutput;
    }
}

template <typename ElementType>
DataStatistics GetWeightsStatistics(const WeightsAndBias<ElementType>& weightsAndBias)
{
    return GetWeightsStatistics(weightsAndBias.weights);
}

template <typename ElementType>
DataStatistics GetWeightsStatistics(const ell::math::RowMatrix<ElementType>& weights)
{
    UnlabeledDataContainer container;
    for (size_t i = 0; i < weights.NumRows(); ++i)
    {
        container.Add(CastVector<float>(weights.GetRow(i)));
    }

    return GetScalarDataStatistics(container);
}

template <typename ElementType>
DataStatistics GetWeightsStatistics(const typename predictors::neural::ConvolutionalLayer<ElementType>::TensorType& weights)
{
    UnlabeledDataContainer container;
    container.Add(CastVector<float>(math::RowVector<ElementType>(weights.ToArray())));
    return GetScalarDataStatistics(container);
}
} // namespace ell
