////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelGenerateArguments.h (utilities)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

namespace ell
{
/// <summary> A struct that holds command line parameters for generating models. </summary>
struct ModelGenerateArguments
{
    /// <summary> The output type to generate. </summary>
    enum class OutputType
    {
        model,
        map
    };
    OutputType outputType;
    std::string outputPath;
};

/// <summary> A version of ModelGenerateArguments that adds its members to the command line parser. </summary>
struct ParsedModelGenerateArguments : public ModelGenerateArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    void AddArgs(utilities::CommandLineParser& parser) override;
};
}
