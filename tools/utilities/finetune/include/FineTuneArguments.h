////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FineTuneArguments.h (linearTrainer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OptimizationUtils.h"
#include "Report.h"

#include <common/include/DataLoadArguments.h>
#include <common/include/MapLoadArguments.h>
#include <common/include/MapSaveArguments.h>
#include <common/include/TrainerArguments.h>

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <string>

namespace ell
{
enum class TargetNodeType : unsigned int
{
    none = 0b0000,
    fullConvolution = 0b0001,
    spatialConvolution = 0b0010,
    pointwiseConvolution = 0b0100,
    fullyConnected = 0b1000
};

struct TargetNodeFlags
{
    TargetNodeFlags(unsigned int flags) :
        flags(flags) {}
    TargetNodeFlags(TargetNodeType t) :
        flags(static_cast<unsigned int>(t)) {}
    unsigned int flags;
};

TargetNodeFlags operator|(TargetNodeType t1, TargetNodeType t2);
TargetNodeFlags operator|(TargetNodeFlags t1, TargetNodeType t2);
TargetNodeFlags operator|(TargetNodeType t1, TargetNodeFlags t2);
TargetNodeFlags operator|(TargetNodeFlags t1, TargetNodeFlags t2);
bool operator&(TargetNodeType t1, TargetNodeType t2);
bool operator&(TargetNodeFlags t1, TargetNodeType t2);
bool operator&(TargetNodeType t1, TargetNodeFlags t2);
TargetNodeFlags operator&(TargetNodeFlags t1, TargetNodeFlags t2);

/// <summary> Arguments for finetune tool. </summary>
struct FineTuneArguments
{
    // Model I/O
    ell::common::ParsedMapLoadArguments mapLoadArguments;
    std::string targetPortElements;
    ell::common::ParsedMapSaveArguments mapSaveArguments;
    bool printModel = false;

    // Train/test data
    int maxTrainingRows = -1;
    int maxTestingRows = -1;
    ell::common::ParsedDataLoadArguments trainDataArguments;
    ell::common::ParsedDataLoadArguments testDataArguments;
    bool multiClass = true;
    std::string dataFormat;
    int maxCacheEntries = 8;

    // Node selection
    int numPrefixNodesToSkip = 0;
    int numSuffixNodesToSkip = 0;
    std::vector<std::string> specificNodesToSkip;
    mutable std::vector<std::string> validNodes;
    bool fineTuneFullyConnectedNodes = true;
    bool fineTuneConvolutionalNodes = true;

    // Optimization parameters
    double l2Regularization = 0;
    double l1Regularization = 0;
    double desiredPrecision = 0;
    double requiredPrecision = 0;
    int maxEpochs = 0;
    LossFunction lossFunction = LossFunction::square;
    bool normalizeInputs = false;
    bool normalizeOutputs = false;
    bool reoptimizeSparseWeights = false;
    bool optimizeFiltersIndependently = false;
    bool permute = true;
    TargetNodeFlags fineTuneTargets = TargetNodeType::fullConvolution | TargetNodeType::pointwiseConvolution | TargetNodeType::fullyConnected;

    // Sparsification parameters
    TargetNodeFlags sparsifyTargets = TargetNodeType::none;
    SparsifyMethod sparsifyMethod = SparsifyMethod::l1;
    double sparsityTarget = 0; // overrides l1 regularization if set
    double sparsityTargetEpsilon = 0.01;

    // Misc
    std::string randomSeed;
    std::string reportFilename;
    Report::ReportFormat reportFormat = Report::ReportFormat::text;
    bool writeOutputStats = false;

    bool testOnly = false;
    bool compile = true;
    bool verbose = false;

    // Helper methods for creating settings objects from arguments
    FineTuneProblemParameters GetFineTuneProblemParameters() const;

    /// <summary> Get the output of the (potentially truncated) model to fine-tune. This is the output our new model will try to match. </summary>
    const ell::model::OutputPortBase& GetInputModelTargetOutput() const;

    /// <summary> Indicate if this node is supposed to be skipped </summary>
    bool SkipNode(std::string id) const;

    ell::utilities::OutputStreamImpostor GetReportStream() const;

    FineTuneArguments() :
        trainDataArguments(ell::common::OptionName{ "trainDataFilename" }, ell::common::OptionName{ "trainDataDirectory" }, ell::common::OptionName{ "trainDataDimension" }),
        testDataArguments(ell::common::OptionName{ "testDataFilename" }, ell::common::OptionName{ "testDataDirectory" }, ell::common::OptionName{ "testDataDimension" })
    {}

private:
    ell::model::Model LoadInputModel() const;
    void ComputeValidNodes(ell::model::Model& model, const ell::model::OutputPortBase& output) const;
    const ell::model::OutputPortBase& GetSpecifiedOutput(ell::model::Model& model) const;

    FineTuneOptimizationParameters GetFineTuneParameters() const;
    FineTuneOptimizationParameters GetSparsifyParameters() const;
    FineTuneOptimizationParameters GetReoptimizeParameters() const;
};

struct ParsedFineTuneArguments
    : public FineTuneArguments
    , public ell::utilities::ParsedArgSet
{
    static ParsedFineTuneArguments ParseCommandLine(int argc, char* argv[]);
    void AddArgs(ell::utilities::CommandLineParser& parser) override;
};
} // namespace ell
