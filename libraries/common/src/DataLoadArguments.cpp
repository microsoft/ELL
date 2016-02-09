////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoadArguments.h"

// utilities
#include "files.h"

// stl
#include <vector>
#include <string>

namespace common
{
    void ParsedDataLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputDataFile,
            "inputDataFile", 
            "idf",
            "Path to the input data file",
            "");

        parser.AddOption(       // TODO this parameter is not used anywhere
            inputDataFileHasWeights,
            "inputDataFileHasWeights",
            "idfhw",
            "Indicates whether the input data file format specifies a weight per example",
            false);
    }

    utilities::ParseResult ParsedDataLoadArguments::PostProcess(const utilities::CommandLineParser & parser)
    {
        std::vector<std::string> parseErrorMessages;
        
        if(inputDataFile == "")
        {
            parseErrorMessages.push_back("-inputDataFile (or -idf) is required");
        }
        else
        {
            if(!utilities::IsFileReadable(inputDataFile))
            {
                parseErrorMessages.push_back("cannot read from specified input data file: " + inputDataFile);
            }
        }

        return parseErrorMessages;
    }
}
