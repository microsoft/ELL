////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoadArguments.h"
#include "DataLoaders.h"

// utilities
#include "Files.h"
#include "Parser.h"

// stl
#include <vector>
#include <string>

namespace common
{
    void ParsedDataLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputDataFilename,
            "inputDataFilename", 
            "idf",
            "Path to the input data file",
            "");

        parser.AddOption(
            dataDimension,
            "dataDimension",
            "dd",
            "Number of elements to read from each data vector",
            "");
    }

    utilities::CommandLineParseResult ParsedDataLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> parseErrorMessages;

        // inputDataFilename
        if (inputDataFilename == "")
        {
            parseErrorMessages.push_back("-inputDataFilename (or -idf) is required");
        }
        else
        {
            if (!utilities::IsFileReadable(inputDataFilename))
            {
                parseErrorMessages.push_back("cannot read from specified input data file: " + inputDataFilename);
            }
        }

        // dataDimension
        const char* ptr = dataDimension.c_str();
        if (dataDimension == "auto")
        {
            auto dataIterator = GetDataIterator(*this);
            while (dataIterator->IsValid())
            {
                auto size = dataIterator->Get().GetDataVector().Size();
                parsedDataDimension = std::max(parsedDataDimension, size);
                dataIterator->Next();
            }
        }
        else if (dataDimension != "")
        {
            utilities::Parse(ptr, parsedDataDimension);
        }

        return parseErrorMessages;
    }
}
