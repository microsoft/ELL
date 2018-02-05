////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareArguments.h (debugCompiler)
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
/// <summary> Command-line arguments for the compare tool. </summary>
struct CompareArguments
{
    // input options
    std::string inputMapFile;
    std::string inputConverter;

    // output options
    std::string outputDirectory = "";
    bool writeReport = true;
    bool writeGraph = true;
    bool writePrediction = true;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedCompareArguments : public CompareArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    void AddArgs(utilities::CommandLineParser& parser) override;
};
}
