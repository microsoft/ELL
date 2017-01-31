////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompileArguments.cpp (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompileArguments.h"

// utilities
#include "Files.h"

namespace ell
{
void ParsedCompileArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(
        outputType,
        "outputType",
        "o",
        "Choice of output type: refinedMap, compiledMap, ir, bc, asm",
        { { "refinedMap", OutputType::refinedMap }, { "compiledMap", OutputType::compiledMap }, { "ir", OutputType::ir }, { "bc", OutputType::bitcode }, { "asm", OutputType::assembly } },
        "ir");

    parser.AddOption(
        outputFilename,
        "outputFilename",
        "of",
        "Path to the output file",
        "");

    parser.AddOption(
        optimize,
        "optimize",
        "opt",
        "Optimize output code",
        false);

    parser.AddOption(
        compiledFunctionName,
        "compiledFunctionName",
        "cfn",
        "Name for compiled function",
        "predict");

    parser.AddOption(
        maxRefinementIterations,
        "maxRefinementIterations",
        "mri",
        "The maximal number of refinement iterations (only valid if outputType is 'refinedMap')",
        10);

    parser.AddOption(
        cpu,
        "cpu",
        "cpu",
        "The CPU target for generating assembly code (only valid if outputType is 'asm')",
        { { "cortex-m0", "cortex-m0" }, { "cortex-m4", "cortex-m4" } },
        "cortex-m0");
}

utilities::CommandLineParseResult ParsedCompileArguments::PostProcess(const utilities::CommandLineParser& parser)
{
    std::vector<std::string> errors;

    // create output stream impostor for code
    if (outputFilename == "null")
    {
        outputCodeStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
    }
    else if (outputFilename == "")
    {
        outputCodeStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
    }
    else // treat argument as filename
    {
        outputCodeStream = utilities::OutputStreamImpostor(outputFilename);
    }

    return errors;
}
}
