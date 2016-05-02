////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataSaveArguments.h (common)
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
    /// <summary> A struct that holds command line parameters for saving data. </summary>
    struct DataSaveArguments
    {
        std::string outputDataFile = "";
    };

    /// <summary> A version of DataSaveArguments that can add its members to the command line parser and post process their values. </summary>
    struct ParsedDataSaveArguments : public DataSaveArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);

        /// <summary> Check the parsed arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::CommandLineParseResult. </returns>
        virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser);
    };
}
