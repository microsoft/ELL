////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataStatistics.h"
#include "FineTuneArguments.h"
#include "FineTuneModel.h"
#include "ModelUtils.h"
#include "Report.h"
#include "TransformData.h"

#include <model/include/OutputPort.h>
#include <model/include/Submodel.h>

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/MillisecondTimer.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <iostream>
#include <stdexcept>

namespace ell
{
// Prototypes
void Run(const FineTuneArguments& args);
void PrintOriginalModels(std::ostream& logStream, const model::OutputPortBase& modelOutput);
void PrintFineTunedModels(std::ostream& logStream, const model::OutputPortBase& fineTunedOutput);
void PrintModelEvaluation(std::ostream& logStream, const model::OutputPortBase& modelOutput, const MultiClassDataContainer& trainingData, const MultiClassDataContainer& testData);
void SaveModel(const model::OutputPortBase& output, const FineTuneArguments& args);

//
// Implementation
//

// This function does all the work --- it's called by main() right after parsing the command-line arguments
void Run(const FineTuneArguments& args)
{
    utilities::MillisecondTimer totalTimer;
    auto& logStream = std::cout;

    utilities::MillisecondTimer loadModelTimer;

    const auto& modelOutput = args.GetInputModelTargetOutput();
    loadModelTimer.Stop();

    if (args.printModel)
    {
        PrintOriginalModels(logStream, modelOutput);
    }

    // Create report
    auto reportStream = args.GetReportStream();
    Report report(reportStream, Report::ReportFormat::text);
    report.WriteParameters(args);

    // Load datasets
    utilities::MillisecondTimer loadDatasetTimer;
    auto trainingData = GetMultiClassTrainingDataset(args);
    auto testData = GetMultiClassTestDataset(args);
    loadDatasetTimer.Stop();
    if (args.testOnly)
    {
        PrintModelEvaluation(logStream, modelOutput, trainingData, testData);
        return;
    }

    if (args.verbose)
    {
        if (!trainingData.IsEmpty())
            logStream << "Training dataset size: " << trainingData.Size() << std::endl;
        if (!testData.IsEmpty())
            logStream << "Test dataset size: " << testData.Size() << std::endl;
    }

    utilities::MillisecondTimer fineTuningTotalTimer;

    // Fine-tune the model and return the output of the new model
    auto fineTunedOutputs = FineTuneNodesInSubmodel(model::Submodel({ &modelOutput }), trainingData, args, [&report](const FineTuningLayerResult& layerResult) {
        report.WriteLayerOptimizationResult(layerResult);
    });
    const auto& fineTunedOutput = *fineTunedOutputs.fineTunedSubmodel.GetOutputs()[0];
    fineTuningTotalTimer.Stop();

    if (args.printModel)
    {
        PrintFineTunedModels(logStream, fineTunedOutput);
    }
    SaveModel(fineTunedOutput, args);

    utilities::MillisecondTimer evalModelTimer;
    report.WriteModelAccuracy("Original", "Train", GetModelAccuracy(modelOutput, trainingData));
    report.WriteModelAccuracy("Original", "Test", GetModelAccuracy(modelOutput, testData));
    report.WriteModelAccuracy("FineTuned", "Train", GetModelAccuracy(fineTunedOutput, trainingData));
    report.WriteModelAccuracy("FineTuned", "Test", GetModelAccuracy(fineTunedOutput, testData));
    evalModelTimer.Stop();

    report.WriteModelSparsity("Original", GetSubmodelWeightsSparsity(model::Submodel{ { &modelOutput } }));
    report.WriteModelSparsity("FineTuned", GetSubmodelWeightsSparsity(model::Submodel{ { &fineTunedOutput } }));

    report.WriteTiming("LoadModelTime", loadModelTimer.Elapsed());
    report.WriteTiming("LoadDatasetsTime", loadDatasetTimer.Elapsed());
    report.WriteTiming("DataTransformTime", fineTunedOutputs.dataTransformTime);
    report.WriteTiming("OptimizationTime", fineTunedOutputs.optimizationTime);
    report.WriteTiming("TotalFineTuningTime", fineTuningTotalTimer.Elapsed());
    report.WriteTiming("EvalModelTime", evalModelTimer.Elapsed());
    report.WriteTiming("TotalTime", totalTimer.Elapsed());
}

void PrintModelEvaluation(std::ostream& logStream, const model::OutputPortBase& modelOutput, const MultiClassDataContainer& trainingData, const MultiClassDataContainer& testData)
{
    if (!trainingData.IsEmpty())
    {
        logStream << "Training dataset size: " << trainingData.Size() << std::endl;
        auto originalTrainingAccuracy = GetModelAccuracy(modelOutput, trainingData);
        logStream << "Original model accuracy (on train set): " << originalTrainingAccuracy << std::endl;
    }

    if (!testData.IsEmpty())
    {
        logStream << "Test dataset size: " << testData.Size() << std::endl;
        auto originalTestAccuracy = GetModelAccuracy(modelOutput, testData);
        logStream << "Original model accuracy (on test set): " << originalTestAccuracy << std::endl;
    }
}

void PrintOriginalModels(std::ostream& logStream, const model::OutputPortBase& modelOutput)
{
    auto model = modelOutput.GetNode()->GetModel();
    logStream << "Original model:" << std::endl;
    model->Print(logStream);
    logStream << "Model to process:" << std::endl;
    model->PrintSubset(logStream, &modelOutput);
}

void PrintFineTunedModels(std::ostream& logStream, const model::OutputPortBase& fineTunedOutput)
{
    auto model = fineTunedOutput.GetNode()->GetModel();
    logStream << "Full model after fine-tuning:" << std::endl;
    model->Print(logStream);

    logStream << "Fine-tuned model:" << std::endl;
    model->PrintSubset(logStream, &fineTunedOutput);
}

void SaveModel(const model::OutputPortBase& output, const FineTuneArguments& args)
{
    if (!args.mapSaveArguments.outputMapFilename.empty())
    {
        SaveModel(output, args.mapSaveArguments.outputMapFilename);
    }
}
} // namespace ell

//
// Main entry point
//
using namespace ell;
int main(int argc, char* argv[])
{
    try
    {
        auto args = ParsedFineTuneArguments::ParseCommandLine(argc, argv);
        Run(args);
    }
    catch (const std::exception& exception)
    {
        std::cerr << "exception: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
