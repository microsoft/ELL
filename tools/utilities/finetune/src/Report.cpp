////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Report.cpp (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Report.h"
#include "FineTuneArguments.h"
#include "TransformData.h"

using namespace ell;

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

void Report::WriteParameters(const FineTuneArguments& args)
{
    WriteKeyValue("TrainingExamples", args.maxTrainingRows);
    WriteKeyValue("TestExamples", args.maxTestingRows);
    WriteKeyValue("L2Regularization", args.l2Regularization);
    WriteKeyValue("DesiredPrecision", args.desiredPrecision);
    WriteKeyValue("MaxEpochs", args.maxEpochs);
    WriteKeyValue("NormalizeInputs", args.normalizeInputs);
    WriteKeyValue("NormalizeOutputs", args.normalizeOutputs);
    WriteKeyValue("FineTuneFullyConnectedLayers", args.fineTuneFullyConnectedNodes);
    WriteKeyValue("FineTuneConvolutionalLayers", args.fineTuneConvolutionalNodes);
    WriteKeyValue("LayersToSkip", args.numNodesToSkip);
    WriteKeyValue("RandomSeed", args.randomSeed);
    // TODO: loss function
}

void Report::WriteOptimizationParameters(const optimization::SDCAOptimizerParameters& params)
{
    WriteKeyValue("Regularization", params.regularizationParameter);
    WriteKeyValue("PermuteData", params.permuteData);
}

void Report::WriteLayerOptimizationInfo(std::string nodeType, std::string id1, std::string id2, const optimization::SDCASolutionInfo& info)
{
    auto prefix = nodeType + "_" + id1 + "_";
    WriteKeyValue(prefix + "NumEpochs", info.numEpochsPerformed);
    WriteKeyValue(prefix + "DualityGap", info.DualityGap());
    WriteKeyValue(prefix + "PrimalObjective", info.primalObjective);
}

void Report::WriteLayerError(std::string nodeType, std::string id1, std::string id2, double error)
{
    // TODO: write this as an object
    // _stream << nodeType << " node " << id1 << " -> " << id2 << " L2 error:\t" << error << std::endl;
    WriteKeyValue(nodeType + "_" + id1 + "_loss", error);
}

void Report::WriteModelAccuracy(model::Model& model, const model::OutputPortBase& modelOutput, std::string modelName, const std::vector<const MultiClassDataContainer*>& datasets, const std::vector<std::string>& datasetNames)
{
    if (datasets.size() != datasetNames.size())
    {
        throw "Dataset array and names array not the same size";
    }

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        auto accuracy = GetModelAccuracy(model, modelOutput, *datasets[i]);
        WriteKeyValue(modelName + "_" + datasetNames[i], accuracy);
    }
}

void Report::WriteTiming(std::string tag, int milliseconds)
{
    WriteKeyValue(tag, milliseconds);
}
