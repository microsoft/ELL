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
    parser.AddDocumentationString("Output options");

    parser.AddOption(
        outputHeader,
        "header",
        "",
        "Write out a header file",
        false);

    parser.AddOption(
        outputIr,
        "ir",
        "",
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
        "Output directory for compiled model files (if none specified, use the input directory",
        "");

    parser.AddDocumentationString("");
    parser.AddDocumentationString("Compiler options");

    parser.AddOption(
        compiledModuleName,
        "compiledModuleName",
        "cmn",
        "Name for compiled module",
        "ELL");

    parser.AddOption(
        compiledFunctionName,
        "compiledFunctionName",
        "cfn",
        "Name for compiled function (if none specified, use <moduleName>_predict",
        "");

    parser.AddOption(
        profile,
        "profile",
        "p",
        "Emit profiling code",
        false);

    parser.AddOption(
        optimize,
        "optimize",
        "opt",
        "Optimize output code",
        true);

    parser.AddOption(
        useBlas,
        "blas",
        "",
        "Emit code that calls BLAS",
        true);

    parser.AddOption(
        foldLinearOperations,
        "foldLinearOps",
        "",
        "Fold sequences of linear operations with constant coefficients into a single operation",
        true);

    parser.AddDocumentationString("");
    parser.AddDocumentationString("Target device options");
    parser.AddOption(
        target,
        "target",
        "t",
        "target name",
        { { "host" }, { "pi0" }, { "pi3" }, {"pi3_64"}, { "mac" }, { "linux" }, { "windows" }, { "ios" }, { "aarch64" } },
        "host");

    parser.AddOption(
        numBits,
        "numBits",
        "b",
        "Number of bits for target [0 == auto]",
        0);

    parser.AddOption(
        cpu,
        "cpu",
        "cpu",
        "The CPU target for generating code",
        "");

    parser.AddOption(
        targetTriple,
        "triple",
        "",
        "The triple describing the target architecture",
        "");

    parser.AddOption(
        targetDataLayout,
        "datalayout",
        "",
        "The string describing the target data layout",
        "");

    parser.AddOption(
        targetFeatures,
        "features",
        "",
        "A string describing target-specific features to enable or disable (these are LLVM attributes, in the format the llc -mattr option uses)",
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
