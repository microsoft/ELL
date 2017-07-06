////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompileArguments.h (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

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
    std::string compiledFunctionName; // defaults to output filename
    std::string compiledModuleName;
    std::string outputDirectory;
    bool verbose = false;

    // model-generation options
    int maxRefinementIterations = 0;
    bool profile = false;

    // compilation options
    bool optimize = true;
    bool useBlas = false;
    bool foldLinearOperations = true;

    // target machine options
    // known target names: host, mac, linux, windows, arm, arm64, ios
    std::string target = "";

    // These can override default setting for a target
    std::string cpu = "";
    int numBits = NumBitsDefault;
    std::string targetTriple = "";
    std::string targetArchitecture = "";
    std::string targetFeatures = "";
    std::string targetDataLayout = "";
};

/// <summary> Parsed command line arguments for the compile executable. </summary>
struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser) override;
};
}