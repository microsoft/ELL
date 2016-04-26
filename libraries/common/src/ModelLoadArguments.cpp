////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     ModelLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelLoadArguments.h"

// utilities
#include "Files.h"

namespace common
{
    void ParsedModelLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputModelFile,
            "inputModelFile",
            "imf",
            "Path to the input model file",
            "");
    }

    utilities::CommandLineParseResult ParsedModelLoadArguments::PostProcess(const utilities::CommandLineParser & parser)
    {
        std::vector<std::string> parseErrorMessages;

        if (inputModelFile == "")
        {
            parseErrorMessages.push_back("-inputModelFile (or -imf) is required");
        }
        else
        {
            if (!utilities::IsFileReadable(inputModelFile))
            {
                parseErrorMessages.push_back("cannot read from specified input model file: " + inputModelFile);
            }
        }

        return parseErrorMessages;
    }

}
