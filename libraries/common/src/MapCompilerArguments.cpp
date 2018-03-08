////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerArguments.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapCompilerArguments.h"

namespace ell
{
namespace common
{
    void ParsedMapCompilerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
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
            "Name for compiled function (if none specified, use <moduleName>_Predict",
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
            fuseLinearOperations,
            "fuseLinearOps",
            "",
            "Fuse sequences of linear operations with constant coefficients into a single operation",
            true);

        parser.AddOption(
            enableVectorization,
            "vectorize",
            "vec",
            "Enable ELL's vectorization",
            false);

        parser.AddOption(
            vectorWidth,
            "vectorWidth",
            "vw",
            "Size of vector units",
            4);

        parser.AddOption(
            parallelize,
            "parallelize",
            "par",
            "Enable ELL's parallelization",
            false);

        parser.AddOption(
            useThreadPool,
            "threadPool",
            "tp",
            "Use thread pool for parallelization (if parallelization enabled)",
            true);

        parser.AddOption(
            maxThreads,
            "threads",
            "th",
            "Maximum num of parallel threads",
            4);

        parser.AddOption(
            debug,
            "debug",
            "dbg",
            "Emit debug code",
            false);

        parser.AddDocumentationString("");
        parser.AddDocumentationString("Target device options");
        parser.AddOption(
            target,
            "target",
            "t",
            "target name",
            { { "host" }, { "pi0" }, { "pi3" }, { "orangepi0" }, { "pi3_64" }, { "mac" }, { "linux" }, { "windows" }, { "ios" }, { "aarch64" }, { "custom" } },
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
    }

    model::MapCompilerOptions MapCompilerArguments::GetMapCompilerOptions(const std::string& modelName) const
    {
        model::MapCompilerOptions settings;
        bool namespaceSpecified = true;
        auto namespacePrefix = compiledModuleName;
        if (namespacePrefix == "")
        {
            namespacePrefix = "ELL";
            namespaceSpecified = false;
        }

        std::string functionName;
        if (compiledFunctionName == "")
        {
            if (namespaceSpecified)
            {
                functionName = namespacePrefix + "_Predict";
            }
            else
            {
                functionName = modelName;
            }
        }
        else
        {
            functionName = namespacePrefix + "_" + compiledFunctionName;
        }

        settings.moduleName = namespacePrefix;
        settings.mapFunctionName = functionName;
        settings.optimizerSettings.fuseLinearFunctionNodes = fuseLinearOperations;
        settings.compilerSettings.optimize = optimize;
        settings.compilerSettings.useBlas = useBlas;
        settings.compilerSettings.allowVectorInstructions = enableVectorization;
        settings.compilerSettings.parallelize = parallelize;
        settings.compilerSettings.vectorWidth = vectorWidth;
        settings.profile = profile;

        if (target != "")
        {
            settings.compilerSettings.targetDevice.deviceName = target;
        }

        if (cpu != "")
        {
            settings.compilerSettings.targetDevice.cpu = cpu;
        }

        if (targetTriple != "")
        {
            settings.compilerSettings.targetDevice.triple = targetTriple;
        }

        if (targetDataLayout != "")
        {
            settings.compilerSettings.targetDevice.dataLayout = targetDataLayout;
        }

        if (targetFeatures != "")
        {
            settings.compilerSettings.targetDevice.features = targetFeatures;
        }

        if (numBits != 0)
        {
            settings.compilerSettings.targetDevice.numBits = numBits;
        }

        return settings;
    }
}
}
