////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EvaluatorArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// evaluators
#include "Evaluator.h"

namespace ell
{
namespace common
{
    using EvaluatorArguments = evaluators::EvaluatorParameters;

    /// <summary> Parsed version of evaluator arguments. </summary>
    struct ParsedEvaluatorArguments : public EvaluatorArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
