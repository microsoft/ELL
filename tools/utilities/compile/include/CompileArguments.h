////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompileArguments.h (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"
#include "OutputStreamImpostor.h"

// stl
#include <string>

/// <summary> Command line arguments for the compile executable. </summary>
struct CompileArguments
{
    std::string outputCodeFilename;

    utilities::OutputStreamImpostor outputCodeStream;
};

/// <summary> Parsed command line arguments for the compile executable. </summary>
struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputCodeFilename,
            "outputCodeFilename",
            "ocf",
            "Path to the output code file",
            "");
    }

    /// <summary> Check arguments. </summary>
    ///
    /// <param name="parser"> The parser. </param>
    ///
    /// <returns> An utilities::CommandLineParseResult. </returns>
    virtual utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser)
    {
        if(outputCodeFilename == "null")
        {
            outputCodeStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
        }
        else if(outputCodeFilename == "cout")
        {
            outputCodeStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
        }
        else // treat argument as filename
        {
            outputCodeStream = utilities::OutputStreamImpostor(outputCodeFilename);
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
};
