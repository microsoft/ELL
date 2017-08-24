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
/// <summary> Command-line arguments for the compare tool. </summary>
struct CompareArguments
{
    // input options
    std::string inputMapFile;
    std::string inputTestFile;
    float inputScale = 1 / 255;

    // output options
    std::string outputDirectory = "";
    bool writeReport = false;
    bool writeGraph = false;

    // compilation parameters
    bool optimize = true;
    bool useBlas = false;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedCompareArguments : public CompareArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
}
