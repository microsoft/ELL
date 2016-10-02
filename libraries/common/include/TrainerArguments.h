////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace emll
{
namespace common
{

    /// <summary> A struct that holds command line arguments that specify the loss function. </summary>
    struct LossArguments
    {
        enum class LossFunction
        {
            squared,
            log,
            hinge
        };

        LossFunction lossFunction = LossFunction::squared;
        double lossFunctionParameter = 0.0;
    };

    /// <summary> A struct that holds general command line parameters for training algorithms. </summary>
    struct TrainerArguments
    {
        /// <summary> The loss arguments. </summary>
        LossArguments lossArguments;

        /// <summary> A random seed. </summary>
        std::string randomSeedString = "";

        /// <summary> Generate verbose output. </summary>
        bool verbose = false;
    };

    /// <summary> A version of TrainerArguments that adds its members to the command line parser. </summary>
    struct ParsedTrainerArguments : public TrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
