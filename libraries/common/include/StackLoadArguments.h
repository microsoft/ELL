////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     StackLoadArguments.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// <summary> A struct that holds command line parameters for loading maps. </summary>
    struct StackLoadArguments
    {
        std::string inputStackFile = "";
    };

    /// <summary> A version of MapLoadArguments that adds its members to the command line parser. </summary>
    struct ParsedStackLoadArguments : public StackLoadArguments, public utilities::ParsedArgSet
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
