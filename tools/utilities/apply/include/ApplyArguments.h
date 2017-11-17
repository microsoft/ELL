////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ApplyArguments.h (apply)
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
/// <summary> Command line arguments for the apply executable. </summary>
struct ApplyArguments
{
    /// <summary> Path to a second map, whose output values are subtracted from the The output code file. </summary>
    std::string inputMapFilename2;

    /// <summary> Instead of raw output, report a summary. </summary>
    bool summarize = false;
};

/// <summary> Parsed command line arguments for the apply executable. </summary>
struct ParsedApplyArguments : public ApplyArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    void AddArgs(utilities::CommandLineParser& parser) override;

    /// <summary> Check the parsed arguments. </summary>
    ///
    /// <param name="parser"> The parser. </param>
    ///
    /// <returns> An utilities::CommandLineParseResult. </returns>
    utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser) override;
};
}