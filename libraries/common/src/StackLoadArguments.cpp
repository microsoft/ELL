////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     StackLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StackLoadArguments.h"

// utilities
#include "Files.h"

namespace common
{
    void ParsedStackLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputStackFile,
            "inputStackFile",
            "isf",
            "Path to the input stack file",
            "");
    }

    utilities::CommandLineParseResult ParsedStackLoadArguments::PostProcess(const utilities::CommandLineParser & parser)
    {
        std::vector<std::string> parseErrorMessages;

        if (inputStackFile == "")
        {
            parseErrorMessages.push_back("-inputStackFile (or -isf) is required");
        }
        else
        {
            if (!utilities::IsFileReadable(inputStackFile))
            {
                parseErrorMessages.push_back("cannot read from specified input stack file: " + inputStackFile);
            }
        }

        return parseErrorMessages;
    }

}
