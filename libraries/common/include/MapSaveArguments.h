////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapSaveArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// <summary> A struct that holds command line parameters for saving maps. </summary>
    struct MapSaveArguments
    {
        std::string outputModelFile = "";
    };

    /// <summary> A version of MapSaveArguments that adds its members to the command line parser. </summary>
    struct ParsedMapSaveArguments : public MapSaveArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}
