////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataSaveArguments.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// <summary> A struct that holds command line parameters for loading maps. </summary>
    struct DataSaveArguments
    {
        std::string outputDataFile = "";
    };

    /// <summary> A version of DataSaveArguments that adds its members to the command line parser. </summary>
    struct ParsedDataSaveArguments : public DataSaveArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);

        /// <summary> Check arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::ParseResult. </returns>
        virtual utilities::ParseResult PostProcess(const utilities::CommandLineParser& parser);
    };
}
