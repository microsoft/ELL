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
/// <summary> Command line arguments for the compile executable. </summary>
struct CompileArguments
{
    /// <summary> The output type to generate. </summary>
    enum class OutputType
    {
        refinedMap,
        compiledMap,
        ir,
        bitcode,
        assembly,
        swigInterface
    };
    OutputType outputType;

    /// <summary> The output code file. </summary>
    /// <remarks>
    /// Used as the base filename if the outputType is 'swigInterface'
    /// </remarks>
    std::string outputFilename;

    /// <summary> true to optimize. </summary>
    bool optimize = false;

    /// <summary> Name of the compiled function. </summary>
    std::string compiledFunctionName;

    /// <summary> The maximum number of refinement outputTypes. </summary>
    int maxRefinementIterations = 0;

    /// <summary> An output stream for the output data file. </summary>
    utilities::OutputStreamImpostor outputCodeStream;

    /// <summary> If output type is ASM then we need a target cpu (cortex-m0 or cortex-m4). </summary>
    std::string cpu = "";

    /// <summary> Name of the compiled module. </summary>
    std::string compiledModuleName;
};

/// <summary> Parsed command line arguments for the compile executable. </summary>
struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser) override;

    /// <summary> Check the parsed arguments. </summary>
    ///
    /// <param name="parser"> The parser. </param>
    ///
    /// <returns> An utilities::CommandLineParseResult. </returns>
    virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser) override;
};
}