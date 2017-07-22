////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareArguments.h (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"
#include "OutputStreamImpostor.h"

// stl
#include <ios>
#include <locale>
#include <string>

namespace ell
{
/// <summary> Arguments for print. </summary>
struct CompareArguments
{
    std::string inputMapFile;
    std::string inputTestFile;
    std::string outputDirectory;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedCompareArguments : public CompareArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);

    /// <summary> Check arguments. </summary>
    ///
    /// <param name="parser"> The parser. </param>
    ///
    /// <returns> An utilities::CommandLineParseResult. </returns>
    virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser);
};
}
