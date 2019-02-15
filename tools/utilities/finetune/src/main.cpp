////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FineTuneModel.h"
#include "ModelUtils.h"
#include "Report.h"
#include "TransformData.h"

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/Exception.h>
#include <utilities/include/MillisecondTimer.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <iostream>

using namespace ell;

// Prototypes
void Run(const FineTuneArguments& args);
void PrintOriginalModels(std::ostream& logStream, model::Model& model, const model::OutputPortBase& modelOutput);
void PrintFineTunedModels(std::ostream& logStream, model::Model& model, const model::OutputPortBase& fineTunedOutput);
void PrintModelEvaluation(std::ostream& logStream, model::Model& model, const model::OutputPortBase& modelOutput, const MultiClassDataContainer& trainingData, const MultiClassDataContainer& testData);
void SaveModel(const ell::model::Model& model,
               const ell::model::OutputPortBase& output,
               const FineTuneArguments& args);

// Implementation
int main(int argc, char* argv[])
{
    try
    {
        auto args = ParseCommandLine(argc, argv);
        Run(args);
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

void Run(const FineTuneArguments& args)
{
    utilities::MillisecondTimer totalTimer;
    auto& logStream = std::cout;
    auto reportStream = args.GetReportStream();
    Report report(reportStream, Report::ReportFormat::text);

    utilities::MillisecondTimer loadModelTimer;

    auto model = LoadInputModel(args);
    const auto& modelOutput = GetInputModelTargetOutput(model, args);
    loadModelTimer.Stop();

    if (args.print)
    {
        PrintOriginalModels(logStream, model, modelOutput);
    }

    // Load datasets
    utilities::MillisecondTimer loadDatasetTimer;
    auto trainingData = GetMultiClassTrainingDataset(args);
    auto testData = GetMultiClassTestDataset(args);
    loadDatasetTimer.Stop();
    if (args.testOnly)
    {
        PrintModelEvaluation(logStream, model, modelOutput, trainingData, testData);
        return;
    }

    if (args.verbose)
    {
        if (!trainingData.IsEmpty())
            logStream << "Training dataset size: " << trainingData.Size() << std::endl;
        if (!testData.IsEmpty())
            logStream << "Test dataset size: " << testData.Size() << std::endl;
    }

    // Fine-tune the model and save it
    utilities::MillisecondTimer fineTuningTotalTimer;
    auto fineTunedOutputs = FineTuneNodesInSubmodel(model, modelOutput, trainingData, args);
    const auto& fineTunedOutput = *fineTunedOutputs.fineTunedSubmodel.GetOutputs()[0];
    fineTuningTotalTimer.Stop();

    if (args.print)
    {
        PrintFineTunedModels(logStream, model, fineTunedOutput);
    }
    SaveModel(model, fineTunedOutput, args);

    // Write out report
    report.WriteParameters(args);
    for (auto layerInfo : fineTunedOutputs.layerResults)
    {
        WriteLayerOptimizationResult(*layerInfo.originalOutput, *layerInfo.fineTunedOutput, layerInfo.optimizationInfo, report);
    }

    utilities::MillisecondTimer evalModelTimer;
    report.WriteModelAccuracy(model, modelOutput, "Original", { &trainingData, &testData }, { "Train", "Test" });
    report.WriteModelAccuracy(model, fineTunedOutput, "FineTuned", { &trainingData, &testData }, { "Train", "Test" });
    evalModelTimer.Stop();

    report.WriteTiming("LoadModelTime", loadModelTimer.Elapsed());
    report.WriteTiming("LoadDatasetsTime", loadDatasetTimer.Elapsed());
    report.WriteTiming("DataTransformTime", fineTunedOutputs.dataTransformTime);
    report.WriteTiming("OptimizationTime", fineTunedOutputs.optimizationTime);
    report.WriteTiming("TotalFineTuningTime", fineTuningTotalTimer.Elapsed());
    report.WriteTiming("EvalModelTime", evalModelTimer.Elapsed());
    report.WriteTiming("TotalTime", totalTimer.Elapsed());
}

void PrintModelEvaluation(std::ostream& logStream, model::Model& model, const model::OutputPortBase& modelOutput, const MultiClassDataContainer& trainingData, const MultiClassDataContainer& testData)
{
    if (!trainingData.IsEmpty())
    {
        logStream << "Training dataset size: " << trainingData.Size() << std::endl;
        auto originalTrainingAccuracy = GetModelAccuracy(model, modelOutput, trainingData);
        logStream << "Original model accuracy (on train set): " << originalTrainingAccuracy << std::endl;
    }

    if (!testData.IsEmpty())
    {
        logStream << "Test dataset size: " << testData.Size() << std::endl;
        auto originalTestAccuracy = GetModelAccuracy(model, modelOutput, testData);
        logStream << "Original model accuracy (on test set): " << originalTestAccuracy << std::endl;
    }
}

void PrintOriginalModels(std::ostream& logStream, model::Model& model, const model::OutputPortBase& modelOutput)
{
    logStream << "Original model:" << std::endl;
    model.Print(logStream);
    logStream << "Model to process:" << std::endl;
    model.PrintSubset(logStream, &modelOutput);
}

void PrintFineTunedModels(std::ostream& logStream, model::Model& model, const model::OutputPortBase& fineTunedOutput)
{
    logStream << "Full model after fine-tuning:" << std::endl;
    model.Print(logStream);

    logStream << "Fine-tuned model:" << std::endl;
    model.PrintSubset(logStream, &fineTunedOutput);
}

void SaveModel(const model::Model& model, const model::OutputPortBase& output, const FineTuneArguments& args)
{
    if (!args.mapSaveArguments.outputMapFilename.empty())
    {
        SaveModel(model, output, args.mapSaveArguments.outputMapFilename);
    }
}
