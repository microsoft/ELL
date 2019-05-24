////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Report.cpp (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Report.h"
#include "FineTuneArguments.h"
#include "FineTuneModel.h"
#include "ModelUtils.h"
#include "TransformData.h"

#include <utilities/include/StringUtil.h>

namespace ell
{
Report::Report(std::ostream& stream, ReportFormat format) :
    _stream(stream)
{
}

Report::~Report()
{
    _stream << "\n"
            << std::endl;
}

template <typename ValueType>
void Report::WriteKeyValue(std::string key, const ValueType& value)
{
    _stream << key << ":\t" << value << "\n";
}

void Report::Flush()
{
    _stream << std::flush;
}

void Report::WriteParameters(const FineTuneArguments& args)
{
    WriteKeyValue("TrainingExamples", args.maxTrainingRows);
    WriteKeyValue("TestExamples", args.maxTestingRows);
    WriteKeyValue("TrainingDataset", args.trainDataArguments.GetDataFilePath());
    WriteKeyValue("TestDataset", args.testDataArguments.GetDataFilePath());
    WriteKeyValue("L2Regularization", args.l2Regularization);
    if (args.sparsityTarget != 0)
    {
        WriteKeyValue("DesiredSparsity", args.sparsityTarget);
        WriteKeyValue("DesiredSparsityEps", args.sparsityTargetEpsilon);
    }
    else if (args.l1Regularization > 0)
    {
        WriteKeyValue("L1Regularization", args.l1Regularization);
    }
    WriteKeyValue("DesiredPrecision", args.desiredPrecision);
    WriteKeyValue("MaxEpochs", args.maxEpochs);
    WriteKeyValue("LossFunction", ToString(args.lossFunction));
    WriteKeyValue("NormalizeInputs", args.normalizeInputs);
    WriteKeyValue("NormalizeOutputs", args.normalizeOutputs);
    WriteKeyValue("ReoptimizeWeights", args.reoptimizeSparseWeights);
    WriteKeyValue("FineTuneFullyConnectedLayers", args.fineTuneFullyConnectedNodes);
    WriteKeyValue("FineTuneConvolutionalLayers", args.fineTuneConvolutionalNodes);
    WriteKeyValue("TrainFiltersIndependently", args.optimizeFiltersIndependently);
    WriteKeyValue("LayersToSkip", args.numPrefixNodesToSkip);
    WriteKeyValue("LayersToSkipAtEnd", args.numSuffixNodesToSkip);
    if (!args.specificNodesToSkip.empty())
    {
        WriteKeyValue("SpecificLayersToSkip", utilities::Join(args.specificNodesToSkip, ","));
    }

    WriteKeyValue("RandomSeed", args.randomSeed);

    Flush();
}

void Report::WriteLayerOptimizationResult(const FineTuningLayerResult& layerInfo)
{
    const model::OutputPortBase& originalOutput = *layerInfo.originalOutput;
    const optimization::SDCASolutionInfo& optimizationInfo = layerInfo.solutionInfo.info;

    std::string nodeType = "";
    if (IsConvolutionalLayerNode(originalOutput.GetNode()))
        nodeType = "Convolutional";
    else if (IsFullyConnectedLayerNode(originalOutput.GetNode()))
        nodeType = "FullyConnected";

    auto nodeId = originalOutput.GetNode()->GetId().ToString();

    WriteLayerOptimizationInfo(nodeType, nodeId, optimizationInfo);
    WriteLayerRegularizationParameters(nodeType, nodeId, layerInfo.solutionInfo.l2Regularization, layerInfo.solutionInfo.l1Regularization);
    WriteLayerStatistics(nodeType, nodeId, "Original", "Weights", layerInfo.statistics.originalWeightsStatistics);
    WriteLayerStatistics(nodeType, nodeId, "Final", "Weights", layerInfo.statistics.finalWeightsStatistics);
    WriteLayerActivationStatistics(nodeType, nodeId, layerInfo.statistics.originalActivationStatistics, layerInfo.statistics.rawFineTunedActivationStatistics, layerInfo.statistics.fineTunedActivationStatistics);
    WriteLayerTiming(nodeType, nodeId, layerInfo.dataTransformTime, layerInfo.optimizationTime);
}

void Report::WriteLayerOptimizationInfo(std::string nodeType, std::string nodeId, const optimization::SDCASolutionInfo& info)
{
    auto prefix = nodeType + "_" + nodeId + "_";
    WriteKeyValue(prefix + "NumEpochs", info.numEpochsPerformed);
    WriteKeyValue(prefix + "DualityGap", info.DualityGap());
    WriteKeyValue(prefix + "PrimalObjective", info.primalObjective);
}

void Report::WriteLayerRegularizationParameters(std::string nodeType, std::string nodeId, double l2Regularization, double l1Regularization)
{
    auto prefix = nodeType + "_" + nodeId + "_";
    WriteKeyValue(prefix + "L2Regularization", l2Regularization);
    WriteKeyValue(prefix + "L1Regularization", l1Regularization);
}

void Report::WriteLayerStatistics(std::string nodeType, std::string nodeId, std::string tag, std::string statsType, const DataStatistics& statistics)
{
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + "Num" + statsType, statistics.sparsity[0].numValues);
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + "NumZero" + statsType, statistics.sparsity[0].numZeros);
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + statsType + "Sparsity", statistics.sparsity[0].GetSparsity());
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + statsType + "Mean", statistics.mean[0]);
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + statsType + "Variance", statistics.variance[0]);
    WriteKeyValue(nodeType + "_" + nodeId + "_" + tag + statsType + "StdDev", statistics.stdDev[0]);
    Flush();
}

void Report::WriteLayerActivationStatistics(std::string nodeType, std::string nodeId, const DataStatistics& originalStatistics, const std::optional<DataStatistics>& unnormalizedFineTunedStatistics, const std::optional<DataStatistics>& fineTunedStatistics)
{
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalNumActivations", originalStatistics.sparsity[0].numValues);
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalNumZeroActivations", originalStatistics.sparsity[0].numZeros);
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalActivationsSparsity", originalStatistics.sparsity[0].GetSparsity());
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalActivationsMean", originalStatistics.mean[0]);
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalActivationsVariance", originalStatistics.variance[0]);
    WriteKeyValue(nodeType + "_" + nodeId + "_OriginalActivationsStdDev", originalStatistics.stdDev[0]);

    if (unnormalizedFineTunedStatistics)
    {
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedNumActivations", unnormalizedFineTunedStatistics.value().sparsity[0].numValues);
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedNumZeroActivations", unnormalizedFineTunedStatistics.value().sparsity[0].numZeros);
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedActivationsSparsity", unnormalizedFineTunedStatistics.value().sparsity[0].GetSparsity());
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedActivationsMean", unnormalizedFineTunedStatistics.value().mean[0]);
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedActivationsVariance", unnormalizedFineTunedStatistics.value().variance[0]);
        WriteKeyValue(nodeType + "_" + nodeId + "_PreNormFineTunedActivationsStdDev", unnormalizedFineTunedStatistics.value().stdDev[0]);
    }

    if (fineTunedStatistics)
    {
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedNumActivations", fineTunedStatistics.value().sparsity[0].numValues);
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedNumZeroActivations", fineTunedStatistics.value().sparsity[0].numZeros);
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedActivationsSparsity", fineTunedStatistics.value().sparsity[0].GetSparsity());
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedActivationsMean", fineTunedStatistics.value().mean[0]);
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedActivationsVariance", fineTunedStatistics.value().variance[0]);
        WriteKeyValue(nodeType + "_" + nodeId + "_FineTunedActivationsStdDev", fineTunedStatistics.value().stdDev[0]);
    }
    Flush();
}

void Report::WriteLayerTiming(std::string nodeType, std::string nodeId, int transformTime, int optimizationTime)
{
    WriteTiming(nodeType + "_" + nodeId + "_DataTransformTime", transformTime);
    WriteTiming(nodeType + "_" + nodeId + "_OptimizationTime", optimizationTime);
}

void Report::WriteModelAccuracy(std::string modelName, std::string datasetName, double accuracy)
{
    WriteKeyValue(modelName + "_" + datasetName + "_Accuracy", accuracy);
}

void Report::WriteModelSparsity(std::string modelName, const Sparsity& sparsity)
{
    WriteKeyValue(modelName + "_TotalWeights", sparsity.numValues);
    WriteKeyValue(modelName + "_TotalZeros", sparsity.numZeros);
    WriteKeyValue(modelName + "_Sparsity", sparsity.GetSparsity());
    Flush();
}

void Report::WriteTiming(std::string tag, int milliseconds)
{
    WriteKeyValue(tag, milliseconds);
}
} // namespace ell
