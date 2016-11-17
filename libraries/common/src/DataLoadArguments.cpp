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
#include <string>
#include <vector>
#include <algorithm>

namespace emll
{
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
        bool isFileReadable = false;
        // inputDataFilename
        if (inputDataFilename != "")
        {
            isFileReadable = utilities::IsFileReadable(inputDataFilename);
        }

        // dataDimension
        const char* ptr = dataDimension.c_str();
        if (dataDimension == "auto")
        {
            if (!isFileReadable)
            {
                parseErrorMessages.push_back("Couldn't read data file");    
                return parseErrorMessages;
            }

            auto dataIterator = GetDataIterator(*this);
            while (dataIterator->IsValid())
            {
                auto size = dataIterator->Get().GetDataVector().PrefixLength();
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
}
