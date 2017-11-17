////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapSaveArguments.h (common)
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
    /// <summary> A struct that holds command line parameters for saving maps. </summary>
    struct MapSaveArguments
    {
        /// <summary> The filename to store the output map in. </summary>
        std::string outputMapFilename = "";

        /// <summary> An output stream to write the output map to. </summary>
        utilities::OutputStreamImpostor outputMapStream;

        /// <summary> Checks if there's a valid output stream </summary>
        bool hasOutputStream = false;
    };

    /// <summary> A version of MapSaveArguments that adds its members to the command line parser. </summary>
    struct ParsedMapSaveArguments : public MapSaveArguments, public utilities::ParsedArgSet
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
