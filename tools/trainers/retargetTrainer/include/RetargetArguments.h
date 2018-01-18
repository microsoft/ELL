////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RetargetArguments.h (linearTrainer)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// common
#include "TrainerArguments.h"

namespace ell
{
    /// <summary> Arguments for retarget. </summary> 
    struct RetargetArguments
    {
        std::string inputModelFilename;
        std::string outputModelFilename;
        std::string inputDataFilename;
        std::string targetPortElements;
        size_t removeLastLayers;
        double desiredPrecision;
        size_t maxEpochs;
        size_t refineIterations;
        std::string randomSeedString;
        bool permute;
        bool normalize;
        double regularization;
        bool verbose;
        bool multiClass;
        common::LossFunctionArguments lossFunctionArguments;
        bool useBlas;
    };

    /// <summary> Parsed version of RetargetArguments. </summary>
    struct ParsedRetargetArguments : public RetargetArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
