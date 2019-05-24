////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FineTuneModel.h (finetune)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataStatistics.h"
#include "DataUtils.h"
#include "ModelUtils.h"
#include "OptimizationUtils.h"

#include <model/include/Model.h>
#include <model/include/OutputPort.h>
#include <model/include/Submodel.h>

#include <chrono>
#include <optional>
#include <vector>

namespace ell
{
// Types
class Report;
struct FineTuneArguments;
class ModelOutputDataCache;

/// <summary> The actions that may be applied to a node. </summary>
enum class FineTuneNodeAction
{
    copy,
    finetune,
    sparsify,
    reoptimize,
    none
};

std::string ToString(FineTuneNodeAction action);

/// <summary> A dataset for fine-tuning (in the future, this will include statistics for normalization). </summary>
template <typename ElementType>
struct FineTuningDataset
{
    VectorLabelDataContainer dataset;
    DataStatistics labelStats;
    const ell::model::OutputPort<ElementType>* normalizedFeaturesOutput;
};

/// <summary> Statistics about the fine-tuning process </summary>
struct FineTuningStats
{
    DataStatistics originalWeightsStatistics;
    std::optional<DataStatistics> sparsifiedWeightsStatistics; // weights from sparsifying weights
    std::optional<DataStatistics> reoptimizedWeightsStatistics; // weights from reoptimizing sparse weights
    std::optional<DataStatistics> failedWeightsStatistics; // weights from an optimization that didn't converge
    DataStatistics finalWeightsStatistics; // result fine-tuned (result) weights

    DataStatistics originalActivationStatistics;
    std::optional<DataStatistics> rawFineTunedActivationStatistics;
    std::optional<DataStatistics> fineTunedActivationStatistics;
};

/// <summary> Various outputs and statistics from fine-tuning a layer. </summary>
struct FineTuningLayerResult
{
    bool valid = true;
    const ell::model::OutputPortBase* originalOutput;
    const ell::model::OutputPortBase* fineTunedOutput;

    SolutionInfo solutionInfo;
    FineTuningStats statistics;
    std::chrono::milliseconds::rep dataTransformTime;
    std::chrono::milliseconds::rep optimizationTime;
};

/// <summary> Various outputs and statistics from the fine-tuning process. </summary>
struct FineTuningResult
{
    std::vector<FineTuningLayerResult> layerResults;
    ell::model::Submodel fineTunedSubmodel;
    std::chrono::milliseconds::rep dataTransformTime;
    std::chrono::milliseconds::rep optimizationTime;
};

// Functions

// Fine-tuning nodes

/// <summary> Run the fine-tuning process on a submodel, regenerating weights for any fully-connected or convolutional layers. </summary>
FineTuningResult FineTuneNodesInSubmodel(const ell::model::Submodel& submodel,
                                         MultiClassDataContainer& trainingData,
                                         const FineTuneArguments& args,
                                         std::function<void(const FineTuningLayerResult&)> layerCallback = nullptr);

/// <summary> Train a new layer to approximate the output of a submodel, using the training data supplied. </summary>
template <typename ElementType, typename LayerParametersType>
FineTuningLayerResult ApproximateSubmodelWithNewLayer(const MultiClassDataContainer& imageData,
                                                      const LayerParametersType& convParams,
                                                      const ell::model::OutputPort<ElementType>& submodelOutput,
                                                      const ell::model::OutputPort<ElementType>& destination,
                                                      FineTuneNodeAction action,
                                                      const FineTuneProblemParameters& optimizerParameters,
                                                      ModelOutputDataCache& dataCache);

// Dataset-related
/// <summary> Load the appropriate test or training dataset </summary>
///
/// <param name="args"> The command-line arguments. </param>
/// @{
MultiClassDataContainer GetMultiClassTrainingDataset(const FineTuneArguments& args);
BinaryLabelDataContainer GetBinaryTrainingDataset(const FineTuneArguments& args);
MultiClassDataContainer GetMultiClassTestDataset(const FineTuneArguments& args);
BinaryLabelDataContainer GetBinaryTestDataset(const FineTuneArguments& args);
/// @}

/// <summary> Get a dataset appropriate for re-optimizing the weights in a convolutional or fully-connected layer. </summary>
///
/// <param name="submodelOutput"> The output of the layer we're replacing. </param>
/// <param name="layerParams"> The parameters for the layer. </param>
/// <param name="submodelOutput"> The output of the layer being replaced. </param>
/// <param name="destination"> The destination where the new nodes will be grafted onto (the input to the layer being replaced). </param>
/// <param name="normalizeInputs"> If true, normalize inputs to fine-tuned layer </param>
/// <param name="normalizeOutputs"> If true, renormalize outputs of fine-tuned layer to match statistics of original  </param>
/// <param name="cache"> A data cache for caching intermediate model outputs </param>
/// @{
template <typename ElementType>
FineTuningDataset<ElementType> GetFineTuningDataset(const MultiClassDataContainer& imageData,
                                                    const FullyConnectedParameters& layerParams,
                                                    const ell::model::OutputPort<ElementType>& submodelOutput,
                                                    const ell::model::OutputPort<ElementType>& destination,
                                                    bool normalizeFeatures,
                                                    bool normalizeLabels,
                                                    ModelOutputDataCache& dataCache);

template <typename ElementType>
FineTuningDataset<ElementType> GetFineTuningDataset(const MultiClassDataContainer& imageData,
                                                    const ConvolutionalParameters& layerParams,
                                                    const ell::model::OutputPort<ElementType>& submodelOutput,
                                                    const ell::model::OutputPort<ElementType>& destination,
                                                    bool normalizeFeatures,
                                                    bool normalizeLabels,
                                                    ModelOutputDataCache& dataCache);
/// @}
} // namespace ell
