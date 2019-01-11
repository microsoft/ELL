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

/// <summary> Arguments for finetune. </summary>
struct FineTuneArguments
{
    ell::common::ParsedMapLoadArguments mapLoadArguments;
    std::string targetPortElements;
    int numNodesToSkip = 0;

    bool fineTuneFullyConnectedNodes = true;
    bool fineTuneConvolutionalNodes = true;

    ell::common::ParsedMapSaveArguments mapSaveArguments;

    int maxTrainingRows = -1;
    int maxTestingRows = -1;
    ell::common::ParsedDataLoadArguments trainDataArguments;
    ell::common::ParsedDataLoadArguments testDataArguments;
    std::string dataFormat;

    bool multiClass = true;

    double desiredPrecision = 0;
    int maxEpochs = 0;
    double l2Regularization = 0;
    double l1Regularization = 0;
    std::string randomSeed;
    bool permute = false;
    bool normalizeInputs = false;
    bool normalizeOutputs = false;

    std::string reportFilename;

    bool compile = true;
    bool print = false;
    bool testOnly = false;
    bool verbose = false;

    FineTuneOptimizationParameters GetOptimizerParameters() const;
    ell::utilities::OutputStreamImpostor GetReportStream() const;

    FineTuneArguments() :
        trainDataArguments(ell::common::OptionName{ "trainDataFilename" }, ell::common::OptionName{ "trainDataDirectory" }, ell::common::OptionName{ "trainDataDimension" }),
        testDataArguments(ell::common::OptionName{ "testDataFilename" }, ell::common::OptionName{ "testDataDirectory" }, ell::common::OptionName{ "testDataDimension" })
    {}
};

struct ParsedFineTuneArguments
    : public FineTuneArguments
    , public ell::utilities::ParsedArgSet
{
    void AddArgs(ell::utilities::CommandLineParser& parser) override;
};

ParsedFineTuneArguments ParseCommandLine(int argc, char* argv[]);
