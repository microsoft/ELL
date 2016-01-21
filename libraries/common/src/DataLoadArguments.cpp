// DataLoadArguments.cpp

#include "DataLoadArguments.h"

// utilities
#include "files.h"
using utilities::IsFileReadable;

// stl
#include <vector>
using std::vector;

#include <string>
using std::string;

namespace common
{
    void ParsedDataLoadArguments::AddArgs(CommandLineParser& parser)
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

    ParseResult ParsedDataLoadArguments::PostProcess(const CommandLineParser & parser)
    {
        vector<string> parseErrorMessages;
        
        if(inputDataFile == "")
        {
            parseErrorMessages.push_back("-inputDataFile (or -idf) is required");
        }
        else
        {
            if(!IsFileReadable(inputDataFile))
            {
                parseErrorMessages.push_back("cannot read from specified input data file: " + inputDataFile);
            }
        }

        return parseErrorMessages;
    }
}
