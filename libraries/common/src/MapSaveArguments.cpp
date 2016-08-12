////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapSaveArguments.h"

namespace common
{
    void ParsedMapSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputModelFilename,
            "outputModelFilename",
            "omf",
            "Path to the output model file",
            "");
    }

    utilities::CommandLineParseResult ParsedMapSaveArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        if(outputModelFilename == "null")
        {
            outputModelStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
        }
        else if(outputModelFilename == "")
        {
            outputModelStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
        }
        else // treat argument as filename
        {
            outputModelStream = utilities::OutputStreamImpostor(outputModelFilename);
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
}
