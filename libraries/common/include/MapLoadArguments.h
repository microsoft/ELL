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

namespace common
{
    /// <summary> A struct that holds command line parameters for loading maps. </summary>
    struct MapLoadArguments
    {
        /// <summary> The model load arguments. </summary>
        ParsedModelLoadArguments modelLoadArguments;

        /// <summary> The coordinates from the model to use as output. </summary>
        std::string coordinateListString = "";
    };

    /// <summary> A version of MapLoadArguments that adds its members to the command line parser. </summary>
    struct ParsedMapLoadArguments : public MapLoadArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);

        /// <summary> Check arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::CommandLineParseResult. </returns>
        virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser);

    };
}
