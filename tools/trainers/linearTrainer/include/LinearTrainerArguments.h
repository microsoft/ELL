////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearTrainerArguments.h (linearTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

namespace ell
{
    struct LinearTrainerArguments
    {
        enum class Algorithm
        {
            SGD,
            SparseDataSGD,
            SparseDataCenteredSGD,
            SDCA
        };

    Algorithm algorithm = Algorithm::SGD;
    bool normalize;
    double regularization;
    double desiredPrecision;
    size_t maxEpochs;
    bool permute;
    std::string randomSeedString;
};

/// <summary> Parsed version of LinearTrainerArguments. </summary>
struct ParsedLinearTrainerArguments : public LinearTrainerArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The command line parser. </param>
    void AddArgs(utilities::CommandLineParser& parser) override;
};
}
