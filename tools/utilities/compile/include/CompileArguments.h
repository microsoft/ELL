////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompileArguments.h (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "MapCompiler.h"

// utilities
#include "CommandLineParser.h"
#include "OutputStreamImpostor.h"

// stl
#include <string>

namespace ell
{
/// <summary> Default sentinel value that instructs the compiler to choose the # of bits to use. </summary>
constexpr int NumBitsDefault = 0;

/// <summary> Command line arguments for the compile executable. </summary>
struct CompileArguments
{
    // output options
    bool outputHeader = false;
    bool outputIr = false;
    bool outputBitcode = false;
    bool outputAssembly = false;
    bool outputObjectCode = false;
    bool outputSwigInterface = false;
    bool outputRefinedMap = false;
    bool outputCompiledMap = false;
    std::string outputDirectory;
    std::string outputFilenameBase;
    bool verbose = false;

    // model-generation options
    int maxRefinementIterations = 0;
};

/// <summary> Parsed command line arguments for the compile executable. </summary>
struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    void AddArgs(utilities::CommandLineParser& parser) override;

    model::MapCompilerOptions GetMapCompilerOptions(const std::string& baseFilename);
};
}