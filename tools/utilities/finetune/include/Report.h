////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Report.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <model/include/Model.h>
#include <model/include/OutputPort.h>

// trainers / optimization
#include <optimization/include/SDCAOptimizer.h>

#include <ostream>
#include <vector>

struct FineTuneArguments;

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

    void WriteParameters(const FineTuneArguments& args);
    void WriteOptimizationParameters(const ell::optimization::SDCAOptimizerParameters& params);
    void WriteLayerOptimizationInfo(std::string nodeType, std::string id1, std::string id2, const ell::optimization::SDCASolutionInfo& info);
    void WriteLayerError(std::string nodeType, std::string id1, std::string id2, double error);
    void WriteModelAccuracy(ell::model::Model& model, const ell::model::OutputPortBase& modelOutput, std::string modelName, const std::vector<const MultiClassDataContainer*>& datasets, const std::vector<std::string>& datasetNames);
    void WriteTiming(std::string tag, int milliseconds);

private:
    template <typename ValueType>
    void WriteKeyValue(std::string key, const ValueType& value);

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
