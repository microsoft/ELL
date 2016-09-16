////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

namespace emll
{
namespace common
{
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        modelLoadArguments.AddArgs(parser);

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

        parser.AddOption(
            inputMapFile,
            "inputMapFile",
            "imap",
            "Path to the input map file",
            "");
    }

    utilities::CommandLineParseResult ParsedMapLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        return modelLoadArguments.PostProcess(parser);
    }
}
}
