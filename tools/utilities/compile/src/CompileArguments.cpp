////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompileArguments.cpp (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompileArguments.h"

namespace ell
{
void ParsedCompileArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddDocumentationString("Output options");

    parser.AddOption(
        outputHeader,
        "header",
        "hh",
        "Write out a header file",
        false);

    parser.AddOption(
        outputIr,
        "ir",
        "ir",
        "Write out an LLVM IR (.ll) file",
        false);

    parser.AddOption(
        outputBitcode,
        "bitcode",
        "bc",
        "Write out an LLVM bitcode (.bc) file",
        false);

    parser.AddOption(
        outputAssembly,
        "assembly",
        "asm",
        "Write out an assembly (.s) file",
        false);

    parser.AddOption(
        outputObjectCode,
        "objectCode",
        "obj",
        "Write out an object (.o) file",
        false);

    parser.AddOption(
        outputSwigInterface,
        "swig",
        "",
        "Write out SWIG interfaces for generating language bindings",
        false);

    parser.AddOption(
        outputRefinedMap,
        "refinedMap",
        "",
        "Write out refined map",
        false);

    parser.AddOption(
        outputCompiledMap,
        "compiledMap",
        "",
        "Write out compiled map",
        false);

    parser.AddOption(
        outputDirectory,
        "outputDirectory",
        "od",
        "Output directory for compiled model files (if none specified, use the input directory)",
        "");

    parser.AddOption(
        outputFilenameBase,
        "outputFilenameBase",
        "ob",
        "Base filename for compiled model files (if none specified, use the input model filename)",
        "");

    parser.AddDocumentationString("");
    parser.AddDocumentationString("Misc options");
    parser.AddOption(
        maxRefinementIterations,
        "maxRefinementIterations",
        "mri",
        "The maximal number of refinement iterations (only valid if outputType is 'refinedMap')",
        10);
    parser.AddOption(
        verbose,
        "verbose",
        "v",
        "Print timing information and detail about the network being compiled",
        false);
}
}
