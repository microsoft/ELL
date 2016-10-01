////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

// utilities
#include "Files.h"

namespace emll
{
namespace common
{
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputMapFile,
            "inputMapFile",
            "imap",
            "Path to the input map file",
            "");

        parser.AddOption(
            inputModelFile,
            "inputModelFile",
            "imf",
            "Path to the input model file",
            "");

        parser.AddOption(
            modelInputsString,
            "modelInputs",
            "in",
            "Model inputs to use",
            "");

        parser.AddOption(
            modelOutputsString,
            "modelOutputs",
            "out",
            "Model outputs to use",
            "");
    }

    utilities::CommandLineParseResult ParsedMapLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> parseErrorMessages;

        if (inputModelFile != "" && inputMapFile != "")
        {
            parseErrorMessages.push_back("Can only specify an input map or model file, not both.");
        }
        else if (inputMapFile != "")
        {
            if (!utilities::IsFileReadable(inputMapFile))
            {
                parseErrorMessages.push_back("Cannot read from specified input map file: " + inputMapFile);
            }
        }
        else if (inputModelFile != "")
        {
            if (!utilities::IsFileReadable(inputModelFile))
            {
                parseErrorMessages.push_back("Cannot read from specified input model file: " + inputModelFile);
            }
        }
        else
        {
                parseErrorMessages.push_back("Error: must specify an input map or model file");
        }

        return parseErrorMessages;
    }
}
}
