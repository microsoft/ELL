////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Report.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataStatistics.h"
#include "DataUtils.h"

#include <optimization/include/SDCAOptimizer.h>

#include <ostream>
#include <string>
#include <vector>

namespace ell
{
struct FineTuneArguments;
struct FineTuningLayerResult;

class Report
{
public:
    enum class ReportFormat
    {
        text,
        json
    };

    Report(std::ostream& stream, ReportFormat format);
    ~Report();

    // Parameters
    void WriteParameters(const FineTuneArguments& args);

    // Per-layer info
    void WriteLayerOptimizationResult(const FineTuningLayerResult& layerInfo);

    // Overall result info
    void WriteModelAccuracy(std::string modelName, std::string datasetName, double accuracy);
    void WriteModelSparsity(std::string modelName, const Sparsity& sparsity);
    void WriteTiming(std::string tag, int milliseconds);

private:
    void WriteLayerOptimizationInfo(std::string nodeType, std::string nodeId, const ell::optimization::SDCASolutionInfo& info);
    void WriteLayerRegularizationParameters(std::string nodeType, std::string nodeId, double l2Regularization, double l1Regularization);
    void WriteLayerStatistics(std::string nodeType, std::string nodeId, std::string tag, std::string statsType, const DataStatistics& statistics);
    void WriteLayerActivationStatistics(std::string nodeType, std::string nodeId, const DataStatistics& originalStatistics, const std::optional<DataStatistics>& unnormalizedFineTunedStatistics, const std::optional<DataStatistics>& fineTunedStatistics);
    void WriteLayerTiming(std::string nodeType, std::string nodeId, int transformTime, int optimizationTime);

    template <typename ValueType>
    void WriteKeyValue(std::string key, const ValueType& value);
    void Flush();

    std::ostream& _stream;
};

// Report format:
// Sections: parameters, layers, model accuracy
//   each section is a dict or array of dicts (in JSON)
//
// JSON:
// {
//    params: { 'TrainingExamples': 50, 'Regularization': 0.01, ...},
//    layers: [ { 'Source': '1204', 'Dest': '1489', 'DualityGap': 123.45, 'OptimizationTime': 34.5, ...},
//              { 'Source': '1214', 'Dest': '1532', 'DualityGap': 23.456, ...} ]
//    model: { 'TestAccuracy': 0.32, 'TotalTime': 82643, ...}
// }
//
// Text:
//
// Parameters  (section with dict)
//   TrainingExamples:    50
//   Regularization:      0.01
//
// Layers (section with array of dicts)
//   Source:  1204
//   Dest:    1489
//   DualityGap:  123.45
//   --
//   Source:  1204
//   Dest:    1489
//   DualityGap:  123.45
//
// Model
//   TestAccuracy:    0.32
//   TotalTime:       82643
//
} // namespace ell
