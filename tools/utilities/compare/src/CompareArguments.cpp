////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareArguments.cpp (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompareArguments.h"

// utilities
#include "Files.h"

namespace ell
{
void ParsedCompareArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(inputMapFile, "inputMapFile", "imap", "Path to the input *.map file", "");
    parser.AddOption(inputTestFile, "inputTestFile", "itf", "Path to the input test file containing image to process", "");
    parser.AddOption(outputDirectory, "outputDirectory", "od", "Location of output files (default cwd)", "");
}

utilities::CommandLineParseResult ParsedCompareArguments::PostProcess(const utilities::CommandLineParser& parser)
{
    std::vector<std::string> parseErrorMessages;
    if (inputTestFile == "null" || inputTestFile == "")
    {
        parseErrorMessages.push_back("missing inputTestFile");
    }

    return parseErrorMessages;
}
}
