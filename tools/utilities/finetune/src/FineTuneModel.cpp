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
#include "ModelUtils.h"
#include "OptimizationUtils.h"
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
#include <iostream>
#include <string>
#include <utility>

using namespace ell;
using namespace ell::model;

// Prototypes
const OutputPortBase& GetSpecifiedOutput(model::Model& model, const FineTuneArguments& args);

bool ShouldConsiderLayer(const Node& node, const FineTuneArguments& args);
FineTuningLayerResult RetrainLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters);

FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters);

FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters);

template <typename ElementType>
FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters);

template <typename ElementType>
FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters);

template <typename ElementType>
void WriteModelOutputComparison(model::Model& model, const model::OutputPort<ElementType>& output1, const model::OutputPort<ElementType>& output2, const UnlabeledDataContainer& dataset, Report& report);

// Implementation
template <typename ElementType>
void VerifyConvolutionalWeightsMatrix(const math::RowMatrix<ElementType>& weights, int filterSize, int numInputChannels, int numOutputChannels)
{
    // matrix is f x (k*k*d)
    if (static_cast<int>(weights.NumRows()) != numOutputChannels)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Convolution weights matrix: wrong number of rows");
    }
    if (static_cast<int>(weights.NumColumns()) != filterSize * filterSize * numInputChannels)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Convolution weights matrix: wrong number of columns");
    }
}

Model LoadInputModel(const FineTuneArguments& args)
{
    auto map = common::LoadMap(args.mapLoadArguments);
    auto result = map.GetModel().ShallowCopy();

    // #### TODO: set default convolution method to "unrolled"
    return result;
}

const OutputPortBase& GetInputModelTargetOutput(model::Model& model, const FineTuneArguments& args)
{
    const auto& output = GetSpecifiedOutput(model, args);
    const auto& newOutput = RemoveSourceAndSinkNodes(model, output);
    return newOutput;
}

const OutputPortBase& GetSpecifiedOutput(model::Model& model, const FineTuneArguments& args)
{
    if (args.targetPortElements.empty())
    {
        auto outputNode = GetOutputNode(model);
        return outputNode->GetOutputPort();
    }

    auto targetElementsProxy = model::ParsePortElementsProxy(args.targetPortElements);
    auto targetElements = model::ProxyToPortElements(model, targetElementsProxy);
    if (targetElements.IsFullPortOutput())
    {
        return *(targetElements.GetRanges()[0].ReferencedPort());
    }
    else
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Only full port outputs supported");
    }
}

BinaryLabelDataContainer GetBinaryTrainingDataset(const FineTuneArguments& args)
{
    return LoadBinaryLabelDataContainer(args.trainDataArguments.inputDataFilename);
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

FineTuningOutput FineTuneNodesInSubmodel(Model& model,
                                         const OutputPortBase& submodelOutput,
                                         MultiClassDataContainer& trainingData,
                                         const FineTuneArguments& args)
{
    TransformContext context;
    ModelTransformer transformer;

    std::chrono::milliseconds::rep dataTransformTime = 0;
    std::chrono::milliseconds::rep optimizationTime = 0;
    std::vector<FineTuningLayerResult> layerResults;
    int skipCount = args.numNodesToSkip;
    Submodel submodel(model, {}, { &submodelOutput });
    auto resultSubmodel = transformer.TransformSubmodelOnto(submodel, {}, context, [&skipCount, &layerResults, &dataTransformTime, &optimizationTime, &trainingData, &args](const Node& node, ModelTransformer& transformer) {
        if (ShouldConsiderLayer(node, args))
        {
            if (skipCount > 0)
            {
                transformer.CopyNode(node);
                --skipCount;
            }
            else
            {
                auto retrainingResult = RetrainLayer(transformer, node, trainingData, args.GetOptimizerParameters());
                dataTransformTime += retrainingResult.dataTransformTime;
                optimizationTime += retrainingResult.optimizationTime;
                layerResults.push_back(retrainingResult);
            }
        }
        else
        {
            transformer.CopyNode(node);
        }
    });

    return { resultSubmodel, layerResults, dataTransformTime, optimizationTime };
}

bool ShouldConsiderLayer(const Node& node, const FineTuneArguments& args)
{
    return (args.fineTuneFullyConnectedNodes && IsFullyConnectedLayerNode(&node)) || (args.fineTuneConvolutionalNodes && IsConvolutionalLayerNode(&node));
}

FineTuningLayerResult RetrainLayer(ModelTransformer& transformer,
                                   const Node& node,
                                   MultiClassDataContainer& trainingData,
                                   const FineTuneOptimizationParameters& optimizerParameters)
{
    if (IsFullyConnectedLayerNode(&node)) // TODO: replace with submodel-matcher
    {
        return RetrainFullyConnectedLayer(transformer, node, trainingData, optimizerParameters);
    }
    else if (IsConvolutionalLayerNode(&node)) // TODO: replace with submodel-matcher
    {
        return RetrainConvolutionalLayer(transformer, node, trainingData, optimizerParameters);
    }
    else
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "attempting to return unsupported layer type");
    }
}

FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters)
{
    switch (node.GetOutputPort(0)->GetType())
    {
    case model::Port::PortType::smallReal:
        return RetrainFullyConnectedLayer<float>(transformer, node, trainingData, optimizerParameters);
        break;

    case model::Port::PortType::real:
        return RetrainFullyConnectedLayer<double>(transformer, node, trainingData, optimizerParameters);
        break;

    default:
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Unsupported port type");
    }
}

FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters)
{
    switch (node.GetOutputPort(0)->GetType())
    {
    case model::Port::PortType::smallReal:
        return RetrainConvolutionalLayer<float>(transformer, node, trainingData, optimizerParameters);
        break;

    case model::Port::PortType::real:
        return RetrainConvolutionalLayer<double>(transformer, node, trainingData, optimizerParameters);
        break;

    default:
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Unsupported port type");
    }
}

// Need to pass in training params
template <typename ElementType>
FineTuningLayerResult RetrainFullyConnectedLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters)
{
    // TODO: get input of submodel to retrain

    auto fcNode = dynamic_cast<const nodes::FullyConnectedLayerNode<ElementType>*>(&node);
    const auto& fcOutput = fcNode->output;

    auto& model = transformer.GetModel();
    const auto& destination = transformer.GetCorrespondingOutputs(fcNode->input.GetReferencedPort());
    const auto& fineTunedOutput = ApproximateSubmodelWithFullyConnectedLayer(trainingData, model, fcOutput, destination, optimizerParameters);
    transformer.MapNodeOutput(fcOutput, *fineTunedOutput.fineTunedOutput);
    return fineTunedOutput;
}

// Need to pass in training params
template <typename ElementType>
FineTuningLayerResult RetrainConvolutionalLayer(ModelTransformer& transformer, const Node& node, MultiClassDataContainer& trainingData, const FineTuneOptimizationParameters& optimizerParameters)
{
    // TODO: get input of submodel to retrain

    auto convNode = dynamic_cast<const nodes::ConvolutionalLayerNode<ElementType>*>(&node);
    const auto& convOutput = convNode->output;
    const auto& convLayer = convNode->GetLayer();
    const auto filterSize = static_cast<int>(convLayer.GetConvolutionalParameters().receptiveField);
    const auto stride = static_cast<int>(convLayer.GetConvolutionalParameters().stride);
    const auto inputPadding = convLayer.GetLayerParameters().inputPaddingParameters.paddingSize;
    const auto outputPadding = convLayer.GetLayerParameters().outputPaddingParameters.paddingSize;

    auto& model = transformer.GetModel();
    const auto& destination = transformer.GetCorrespondingOutputs(convNode->input.GetReferencedPort());
    const auto& fineTunedOutput = ApproximateSubmodelWithConvolutionalLayer(trainingData, filterSize, stride, inputPadding, outputPadding, model, convOutput, destination, optimizerParameters);
    transformer.MapNodeOutput(convOutput, *fineTunedOutput.fineTunedOutput);
    return fineTunedOutput;
}

template <typename ElementType>
FineTuningLayerResult ApproximateSubmodelWithFullyConnectedLayer(const MultiClassDataContainer& imageData,
                                                                 Model& model,
                                                                 const OutputPort<ElementType>& submodelOutput,
                                                                 const OutputPort<ElementType>& destination,
                                                                 const FineTuneOptimizationParameters& optimizerParameters)
{
    // Create a dataset from features -> labels
    utilities::MillisecondTimer dataTransformTimer;
    auto retrainingDataset = GetFullyConnectedFineTuningDataset(imageData, model, submodelOutput, destination, optimizerParameters.normalizeInputs, optimizerParameters.normalizeOutputs);
    dataTransformTimer.Stop();

    // Run SDCA to find weights that map data1->data2
    utilities::MillisecondTimer optTimer;
    auto solution = TrainVectorPredictor(std::move(retrainingDataset.dataset), optimizerParameters);
    auto weightsAndBias = GetWeightsAndBias<ElementType>(solution.predictor);
    optTimer.Stop();

    // Add retrained fully connected and bias nodes
    const auto& rawFineTunedOutput = AppendFineTunedFullyConnectedNodes(model, *retrainingDataset.normalizedFeaturesOutput, weightsAndBias);

    auto optimizationTime = optTimer.Elapsed();
    auto dataTransformTime = dataTransformTimer.Elapsed();
    return FineTuningLayerResult{ &submodelOutput, &rawFineTunedOutput, solution.info, dataTransformTime, optimizationTime };
}

template <typename ElementType>
FineTuningDataset<ElementType> GetFullyConnectedFineTuningDataset(const MultiClassDataContainer& imageData,
                                                                  Model& model,
                                                                  const OutputPort<ElementType>& submodelOutput,
                                                                  const OutputPort<ElementType>& destination,
                                                                  bool normalizeInputs,
                                                                  bool normalizeOutputs)
{
    auto imageFeatures = GetDatasetInputs(imageData);

    // Get statistics of "features" and "labels" so we can normalize them for training
    auto rawFeatures = TransformDataWithModel(imageFeatures, model, destination);
    UnlabeledDataContainer trainingFeatures;
    const OutputPort<ElementType>* normalizedFeaturesOutput = nullptr;
    normalizedFeaturesOutput = &destination;
    trainingFeatures = rawFeatures;

    auto rawLabels = TransformDataWithModel(imageFeatures, model, submodelOutput);
    UnlabeledDataContainer trainingLabels;
    trainingLabels = rawLabels;

    // Create a dataset from features -> labels
    auto retrainingDataset = CreateVectorLabelDataContainer(trainingFeatures, trainingLabels);

    return { retrainingDataset, normalizedFeaturesOutput };
}

// TODO: rename this function to imply we're running an optimization
template <typename ElementType>
FineTuningLayerResult ApproximateSubmodelWithConvolutionalLayer(const MultiClassDataContainer& imageData,
                                                                int filterSize,
                                                                int stride,
                                                                int inputPadding,
                                                                int outputPadding,
                                                                Model& model,
                                                                const OutputPort<ElementType>& submodelOutput,
                                                                const OutputPort<ElementType>& destination,
                                                                const FineTuneOptimizationParameters& optimizerParameters)
{
    const auto numChannels = destination.GetMemoryLayout().GetActiveSize()[2];
    const auto numFilters = submodelOutput.GetMemoryLayout().GetActiveSize()[2];

    utilities::MillisecondTimer dataTransformTimer;
    auto retrainingDataset = GetConvolutionalFineTuningDataset(imageData, filterSize, stride, model, submodelOutput, destination, optimizerParameters.normalizeInputs, optimizerParameters.normalizeOutputs);
    dataTransformTimer.Stop();

    // Run SDCA to find weights that map data1->data2
    utilities::MillisecondTimer optTimer;
    auto solution = TrainVectorPredictor(std::move(retrainingDataset.dataset), optimizerParameters);
    auto weightsAndBias = GetWeightsAndBias<ElementType>(solution.predictor);
    optTimer.Stop();
    VerifyConvolutionalWeightsMatrix(weightsAndBias.weights, filterSize, numChannels, numFilters);

    // Add retrained convolutional and bias nodes
    const auto& rawFineTunedOutput = AppendFineTunedConvolutionalNodes(model, *retrainingDataset.normalizedFeaturesOutput, filterSize, stride, inputPadding, outputPadding, weightsAndBias);

    auto optimizationTime = optTimer.Elapsed();
    auto dataTransformTime = dataTransformTimer.Elapsed();
    return FineTuningLayerResult{ &submodelOutput, &rawFineTunedOutput, solution.info, dataTransformTime, optimizationTime };
}

// Rename this function to make it clear we're adding nodes to the model
template <typename ElementType>
FineTuningDataset<ElementType> GetConvolutionalFineTuningDataset(const MultiClassDataContainer& imageData,
                                                                 int filterSize,
                                                                 int stride,
                                                                 Model& model,
                                                                 const OutputPort<ElementType>& submodelOutput,
                                                                 const OutputPort<ElementType>& destination,
                                                                 bool normalizeInputs,
                                                                 bool normalizeOutputs)
{
    auto imageFeatures = GetDatasetInputs(imageData);

    // Get statistics of "features" and "labels" so we can normalize them for training
    auto rawFeatures = TransformDataWithModel(imageFeatures, model, destination);
    UnlabeledDataContainer imageFeatureData;
    const OutputPort<ElementType>* normalizedFeaturesOutput = nullptr;
    normalizedFeaturesOutput = &destination;
    imageFeatureData = rawFeatures;

    auto rawLabels = TransformDataWithModel(imageFeatures, model, submodelOutput);
    UnlabeledDataContainer imageLabelData;
    imageLabelData = rawLabels;

    const auto numRows = destination.GetMemoryLayout().GetExtent()[0];
    const auto numColumns = destination.GetMemoryLayout().GetExtent()[1];
    const auto numChannels = destination.GetMemoryLayout().GetExtent()[2];
    const auto numOutputRows = submodelOutput.GetMemoryLayout().GetActiveSize()[0];
    const auto numOutputColumns = submodelOutput.GetMemoryLayout().GetActiveSize()[1];
    const auto numFilters = submodelOutput.GetMemoryLayout().GetActiveSize()[2];

    UnlabeledDataContainer trainingFeatures = GetUnrolledImageDataset(imageFeatureData, numRows, numColumns, numChannels, filterSize, stride);
    UnlabeledDataContainer trainingLabels = GetImageMatrixDataset(imageLabelData, numOutputRows, numOutputColumns, numFilters);

    // Create a dataset from features -> labels
    auto retrainingDataset = CreateVectorLabelDataContainer(trainingFeatures, trainingLabels);
    return { retrainingDataset, normalizedFeaturesOutput };
}

void WriteLayerOptimizationResult(const model::OutputPortBase& output1, const model::OutputPortBase& output2, const optimization::SDCASolutionInfo& optimizationInfo, Report& report)
{
    std::string nodeType = "";
    if (IsConvolutionalLayerNode(output1.GetNode()))
        nodeType = "Convolutional";
    else if (IsFullyConnectedLayerNode(output1.GetNode()))
        nodeType = "FullyConnected";

    auto nodeId1 = output1.GetNode()->GetId().ToString();
    auto nodeId2 = output2.GetNode()->GetId().ToString();
    report.WriteLayerOptimizationInfo(nodeType, nodeId1, nodeId2, optimizationInfo);
}

void WriteModelOutputComparison(model::Model& model, const model::OutputPortBase& output1, const model::OutputPortBase& output2, const UnlabeledDataContainer& dataset, Report& report)
{
    if (output1.GetType() != output2.GetType())
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Original and fine-tuned outputs have different types");
    }

    switch (output1.GetType())
    {
    case model::Port::PortType::smallReal:
        WriteModelOutputComparison(model, static_cast<const OutputPort<float>&>(output1), static_cast<const OutputPort<float>&>(output2), dataset, report);
        break;

    case model::Port::PortType::real:
        WriteModelOutputComparison(model, static_cast<const OutputPort<double>&>(output1), static_cast<const OutputPort<double>&>(output2), dataset, report);
        break;

    default:
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Unsupported port type");
    }
}

template <typename ElementType>
void WriteModelOutputComparison(model::Model& model, const model::OutputPort<ElementType>& output1, const model::OutputPort<ElementType>& output2, const UnlabeledDataContainer& dataset, Report& report)
{
    auto outputData1 = TransformDataWithModel(dataset, model, output1);
    auto outputData2 = TransformDataWithModel(dataset, model, output2);

    double err = 0;
    auto size = outputData1.Size();
    for (size_t i = 0; i < size; ++i)
    {
        auto vec1 = outputData1[i];
        auto vec2 = outputData2[i];
        vec1 -= vec2;
        err += vec1.Norm2();
    }
    err /= size;

    std::string nodeType = "";
    if (IsConvolutionalLayerNode(output1.GetNode()))
        nodeType = "Convolutional";
    else if (IsFullyConnectedLayerNode(output1.GetNode()))
        nodeType = "FullyConnected";

    auto nodeId1 = output1.GetNode()->GetId().ToString();
    auto nodeId2 = output2.GetNode()->GetId().ToString();
    report.WriteLayerError(nodeType, nodeId1, nodeId2, err);
}

template void WriteModelOutputComparison(model::Model& model, const model::OutputPort<float>& output1, const model::OutputPort<float>& output2, const UnlabeledDataContainer& dataset, Report& report);
template void WriteModelOutputComparison(model::Model& model, const model::OutputPort<double>& output1, const model::OutputPort<double>& output2, const UnlabeledDataContainer& dataset, Report& report);
