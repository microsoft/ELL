////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelLoadArguments.h"

// model
#include "PortElements.h"
#include "Model.h"
#include "InputNode.h"

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

        /// <summary> Query if the arguments specify a map file. </summary>
        ///
        /// <returns> true if the arguments specify a map file. </returns>
        bool HasMapFile() const { return inputMapFile != ""; }
        
        /// <summary> Query if the arguments specify a model file. </summary>
        ///
        /// <returns> true if the arguments specify a model file. </returns>
        bool HasModelFile() const { return inputModelFile != ""; }
        
        /// <summary> Query if the arguments specify either a map file or a model file. </summary>
        ///
        /// <returns> true if the arguments specify a map file or a model file. </returns>
        bool HasInputFile() const { return HasMapFile() || HasModelFile(); }
        
        /// <summary> Get the input node for the loaded model, given the input definition string. </summary>
        ///
        /// <param name="model"> The model as specified by the input model filename </param>
        /// <returns> The specified input node to use for the map. </returns>
        model::InputNodeBase* GetInput(model::Model& model) const;

        /// <summary> Get the output PortElements for the loaded model, given the output definition string. </summary>
        ///
        /// <param name="model"> The model as specified by the input model filename </param>
        /// <returns> The specified output to use for the map. </returns>
        model::PortElementsBase GetOutput(model::Model& model) const;
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
