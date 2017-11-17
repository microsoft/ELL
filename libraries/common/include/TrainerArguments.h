////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace ell
{
namespace common
{

    /// <summary> A struct that holds command line arguments that specify the loss function. </summary>
    struct LossFunctionArguments
    {
        enum class LossFunction
        {
            squared,
            log,
            hinge,
            smoothHinge
        };

        LossFunction lossFunction;
    };

    /// <summary> A struct that holds general command line parameters for training algorithms. </summary>
    struct TrainerArguments
    {
        /// <summary> The loss arguments. </summary>
        LossFunctionArguments lossFunctionArguments;

        /// <summary> Number of epochs. </summary>
        size_t numEpochs;

        /// <summary> Generate verbose output. </summary>
        bool verbose;
    };

    /// <summary> A version of TrainerArguments that adds its members to the command line parser. </summary>
    struct ParsedTrainerArguments : public TrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
