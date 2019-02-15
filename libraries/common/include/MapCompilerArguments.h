////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerArguments.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/MapCompilerOptions.h>
#include <model/include/ModelOptimizerOptions.h>

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/Optional.h>
#include <utilities/include/PropertyBag.h>

#include <string>

namespace ell
{
/// <summary> common namespace </summary>
namespace common
{
    /// <summary> Default sentinel value that instructs the compiler to choose the # of bits to use. </summary>
    constexpr int NumBitsDefault = 0;

    /// <summary> A struct that holds command line parameters for compiling models. </summary>
    struct MapCompilerArguments
    {
        using PreferredConvolutionMethod = model::PreferredConvolutionMethod;

        std::string compilerOptionsFilename;
        std::string compiledFunctionName; // defaults to output filename
        std::string compiledModuleName;

        // ELL codegen options
        bool profile = false;
        bool optimize = true;
        bool useBlas = false;
        bool debug = false;
        utilities::Optional<bool> positionIndependentCode = false; // for generating -fPIC object code

        // potentially per-node options:
        bool enableVectorization = true;
        int vectorWidth = 4;
        bool parallelize = true;
        bool useThreadPool = true;
        int maxThreads = 4;

        // optimization options (configurable per-node)
        bool fuseLinearOperations = true;
        bool optimizeReorderDataNodes = true;
        PreferredConvolutionMethod convolutionMethod = PreferredConvolutionMethod::automatic; // known methods: auto, unrolled, simple, diagonal, winograd

        // raw options to store in metadata
        std::vector<std::string> modelOptions; // in format "<option-name>,<option-value-string>"
        std::vector<std::string> nodeOptions; // in format "<node-id>,<option-name>,<option-value-string>"

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

        /// <summary> Gets a `GetModelOptimizerOptions` with the settings specified in the commandline arguments. </summary>
        ///
        /// <returns> A `model::GetModelOptimizerOptions`. </returns>
        model::ModelOptimizerOptions GetModelOptimizerOptions() const;

        bool HasOptionsMetadata() const;

        /// <summary> Gets a `PropertyBag` with the model- and node-specific settings specified in the commandline arguments. </summary>
        /// This property bag should be appended to the model and node metadata for the model
        ///
        /// <returns> A `PropertyBag` with relevant options for the model and nodes. </returns>
        utilities::PropertyBag GetOptionsMetadata() const;

    private:
        utilities::PropertyBag GetModelOptionsMetadata() const;
        utilities::PropertyBag GetNodeOptionsMetadata() const;
        utilities::PropertyBag LoadOptionsMetadata() const; // load from file
    };

    /// <summary> A version of MapCompilerArguments that adds its members to the command line parser. </summary>
    struct ParsedMapCompilerArguments : public MapCompilerArguments
        , public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
} // namespace common
} // namespace ell
