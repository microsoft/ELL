////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelLoadArguments.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace emll
{
namespace common
{
    /// <summary> A struct that holds command line parameters for loading maps. </summary>
    struct MapLoadArguments
    {
        /// <summary> The file to read a map from. </summary>
        std::string inputMapFile = "";

        /// <summary> The file to read a model from. </summary>
        std::string inputModelFile = "";

        /// <summary> The inputs from the model to use. </summary>
        std::string modelInputsString = "";

        /// <summary> The outputs from the model to use. </summary>
        std::string modelOutputsString = "";

        bool HasMapFile() const { return inputMapFile != ""; }
        bool HasModelFile() const { return inputModelFile != ""; }
    };

    /// <summary> A version of MapLoadArguments that adds its members to the command line parser. </summary>
    struct ParsedMapLoadArguments : public MapLoadArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;

        /// <summary> Check arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::CommandLineParseResult. </returns>
        virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser) override;
    };
}
}
