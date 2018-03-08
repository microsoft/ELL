////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerArguments.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// model
#include "MapCompiler.h"

// stl
#include <string>

namespace ell
{
/// <summary> common namespace </summary>
namespace common
{
    /// <summary> Default sentinel value that instructs the compiler to choose the # of bits to use. </summary>
    constexpr int NumBitsDefault = 0;

    /// <summary> A struct that holds command line parameters for loading data. </summary>
    struct MapCompilerArguments
    {
        std::string compiledFunctionName; // defaults to output filename
        std::string compiledModuleName;

        // ELL codegen options
        bool profile = false;
        bool optimize = true;
        bool useBlas = false;
        bool fuseLinearOperations = true;
        bool enableVectorization = true;
        int vectorWidth = 4;
        bool parallelize = true;
        bool useThreadPool = true;
        int maxThreads = 4;
        bool debug = false;

        // target machine options
        std::string target = ""; // known target names: host, mac, linux, windows, pi0, pi3, pi3_64, aarch64, ios

        // These can override default setting for a target
        std::string cpu = "";
        int numBits = NumBitsDefault;
        std::string targetTriple = "";
        std::string targetArchitecture = "";
        std::string targetFeatures = "";
        std::string targetDataLayout = "";

        /// <summary> Gets a `MapCompilerOptions` with the settings specified in the commandline arguments. </summary>
        ///
        /// <param name="modelname"> The name of the model. </param>
        ///
        /// <returns> A `model::MapCompilerOptions`. </returns>
        model::MapCompilerOptions GetMapCompilerOptions(const std::string& modelName) const;        
    };

    /// <summary> A version of DataLoadArguments that adds its members to the command line parser. </summary>
    struct ParsedMapCompilerArguments : public MapCompilerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
