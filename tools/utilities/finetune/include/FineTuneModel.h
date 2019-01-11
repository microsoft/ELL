////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FineTuneModel.h (finetune)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"
#include "FineTuneArguments.h"
#include "OptimizationUtils.h"
#include "Report.h"

#include <model/include/Model.h>
#include <model/include/OutputPort.h>

#include <chrono>
#include <vector>

// Types

/// <summary> A dataset for fine-tuning (in the future, this will include statistics for normalization). </summary>
template <typename ElementType>
struct FineTuningDataset
{
    VectorLabelDataContainer dataset;
    const ell::model::OutputPort<ElementType>* normalizedFeaturesOutput;
};

/// <summary> Various outputs and statistics from fine-tuning a layer. </summary>
struct FineTuningLayerResult
{
    const ell::model::OutputPortBase* originalOutput;
    const ell::model::OutputPortBase* fineTunedOutput;
    ell::optimization::SDCASolutionInfo optimizationInfo;
    std::chrono::milliseconds::rep dataTransformTime;
    std::chrono::milliseconds::rep optimizationTime;
};

/// <summary> Various outputs and statistics from the fine-tuning process. </summary>
struct FineTuningOutput
{
    ell::model::Submodel fineTunedSubmodel;
    std::vector<FineTuningLayerResult> layerResults;
    std::chrono::milliseconds::rep dataTransformTime;
    std::chrono::milliseconds::rep optimizationTime;
};

// Functions

// Dataset-related
ell::model::Model LoadInputModel(const FineTuneArguments& args);

/// <summary> Get the output of the (potentially truncated) model to fine-tune. This is the output our new model will try to match. </summary>
const ell::model::OutputPortBase& GetInputModelTargetOutput(ell::model::Model& model,
                                                            const FineTuneArguments& args);

MultiClassDataContainer GetMultiClassTrainingDataset(const FineTuneArguments& args);

BinaryLabelDataContainer GetBinaryTrainingDataset(const FineTuneArguments& args);

MultiClassDataContainer GetMultiClassTestDataset(const FineTuneArguments& args);

BinaryLabelDataContainer GetBinaryTestDataset(const FineTuneArguments& args);

/// <summary> Get a dataset appropriate for re-optimizing the weights in a fully-connected layer. </summary>
template <typename ElementType>
FineTuningDataset<ElementType> GetFullyConnectedFineTuningDataset(const MultiClassDataContainer& imageData,
                                                                  ell::model::Model& model,
                                                                  const ell::model::OutputPort<ElementType>& submodelOutput,
                                                                  const ell::model::OutputPort<ElementType>& destination,
                                                                  bool normalizeFeatures,
                                                                  bool normalizeLabels);

/// <summary> Get a dataset appropriate for re-optimizing the weights in a convolutional layer. </summary>
template <typename ElementType>
FineTuningDataset<ElementType> GetConvolutionalFineTuningDataset(const MultiClassDataContainer& imageData,
                                                                 int filterSize,
                                                                 int stride,
                                                                 ell::model::Model& model,
                                                                 const ell::model::OutputPort<ElementType>& submodelOutput,
                                                                 const ell::model::OutputPort<ElementType>& destination,
                                                                 bool normalizeFeatures,
                                                                 bool normalizeLabels);

// Fine-tuning nodes

/// <summary> Run the fine-tuning process on a submodel, regenerating weights for any fully-connected or convolutional layers. </summary>
FineTuningOutput FineTuneNodesInSubmodel(ell::model::Model& model,
                                         const ell::model::OutputPortBase& submodelOutput,
                                         MultiClassDataContainer& trainingData,
                                         const FineTuneArguments& args);

/// <summary> Train a fully-connected layer to approximate the output of a submodel, using the training data supplied. </summary>
template <typename ElementType>
FineTuningLayerResult ApproximateSubmodelWithFullyConnectedLayer(const MultiClassDataContainer& imageData,
                                                                 ell::model::Model& model,
                                                                 const ell::model::OutputPort<ElementType>& submodelOutput,
                                                                 const ell::model::OutputPort<ElementType>& destination,
                                                                 const FineTuneOptimizationParameters& optimizerParameters);

/// <summary> Train a convolutional layer to approximate the output of a submodel, using the training data supplied. </summary>
template <typename ElementType>
FineTuningLayerResult ApproximateSubmodelWithConvolutionalLayer(const MultiClassDataContainer& imageData,
                                                                int filterSize,
                                                                int stride,
                                                                int inputPadding,
                                                                int outputPadding,
                                                                ell::model::Model& model,
                                                                const ell::model::OutputPort<ElementType>& submodelOutput,
                                                                const ell::model::OutputPort<ElementType>& destination,
                                                                const FineTuneOptimizationParameters& optimizerParameters);

// Report generation & model output
void WriteLayerOptimizationResult(const ell::model::OutputPortBase& output1,
                                  const ell::model::OutputPortBase& output2,
                                  const ell::optimization::SDCASolutionInfo& optimizationInfo,
                                  Report& report);

void WriteModelOutputComparison(ell::model::Model& model,
                                const ell::model::OutputPortBase& output1,
                                const ell::model::OutputPortBase& output2,
                                const UnlabeledDataContainer& dataset,
                                Report& report);
