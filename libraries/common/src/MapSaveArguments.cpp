////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapSaveArguments.h"

namespace ell
{
namespace common
{
    void ParsedMapSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputMapFilename,
            "outputMapFilename",
            "omf",
            "Path to the output map file (empty for standard out, 'null' for no output)",
            "");
    }

    utilities::CommandLineParseResult ParsedMapSaveArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        if (outputMapFilename == "null")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
            hasOutputStream = false;
        }
        else if (outputMapFilename == "")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
            hasOutputStream = true;
        }
        else // treat argument as filename
        {
            outputMapStream = utilities::OutputStreamImpostor(outputMapFilename);
            hasOutputStream = true;
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
}
}
