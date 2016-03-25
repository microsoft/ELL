////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     CompileArguments.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

/// <summary> Command line arguments for the compile executable. </summary>
struct CompileArguments
{
    std::string outputCodeFile;
};

/// <summary> Parsed command line arguments for the compile executable. </summary>
struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputCodeFile,
            "outputCodeFile",
            "ocf",
            "Path to the output code file",
            "");
    }
};
