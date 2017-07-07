////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintArguments.h (print)
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
/// <summary> Arguments for print. </summary>
struct PrintArguments
{
    std::string outputFilename;
    std::string outputFormat;
    utilities::OutputStreamImpostor outputStream;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedPrintArguments : public PrintArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
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
