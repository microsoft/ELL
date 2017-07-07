////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintArguments.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"

// utilities
#include "Files.h"

namespace ell
{
void ParsedPrintArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(outputFilename, "outputFilename", "of", "Path to the output file", "");
    parser.AddOption(outputFormat, "outputFormat", "", "What output format to generate [text|dgml] (default text)", "text");
}

utilities::CommandLineParseResult ParsedPrintArguments::PostProcess(const utilities::CommandLineParser& parser)
{
    if (outputFilename == "null")
    {
        outputStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
    }
    else if (outputFilename == "")
    {
        outputStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
    }
    else // treat argument as filename
    {
        outputStream = utilities::OutputStreamImpostor(outputFilename);
    }

    std::vector<std::string> parseErrorMessages;
    return parseErrorMessages;
}
}
