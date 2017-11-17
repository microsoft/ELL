////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataSaveArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"
#include "OutputStreamImpostor.h"

// stl
#include <string>

namespace ell
{
namespace common
{
    /// <summary> A struct that holds command line parameters for saving data. </summary>
    struct DataSaveArguments
    {
        /// <summary> The filename for the output data file. </summary>
        std::string outputDataFilename = "";

        /// <summary> An output stream for the output data file. </summary>
        utilities::OutputStreamImpostor outputDataStream;
    };

    /// <summary> A version of DataSaveArguments that can add its members to the command line parser and post process their values. </summary>
    struct ParsedDataSaveArguments : public DataSaveArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;

        /// <summary> Check the parsed arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::CommandLineParseResult. </returns>
        utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser) override;
    };
}
}
