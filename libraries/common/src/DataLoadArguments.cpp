////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoadArguments.h"
#include "DataLoaders.h"

#include <utilities/include/CStringParser.h>
#include <utilities/include/Files.h>

#include <algorithm>
#include <string>
#include <vector>

namespace ell
{
namespace common
{
    // DataLoadArguments
    std::string DataLoadArguments::GetDataFilePath() const
    {
        return inputDataDirectory.empty() ? inputDataFilename : utilities::JoinPaths(inputDataDirectory, inputDataFilename);
    }

    // ParsedDataLoadArguments
    void ParsedDataLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputDataFilename,
            _filenameOptionString,
            _shortFilenameOptionString,
            "Path to the input data file",
            "");

        if (!_directoryOptionString.empty())
        {
            parser.AddOption(
                inputDataDirectory,
                _directoryOptionString,
                _shortDirectoryOptionString,
                "Directory for the input data file",
                "");
        }

        if (!_dimensionOptionString.empty())
        {
            parser.AddOption(
                dataDimension,
                _dimensionOptionString,
                _shortDimensionOptionString,
                "Number of elements to read from each data vector",
                "");
        }
    }

    ParsedDataLoadArguments::ParsedDataLoadArguments(std::optional<OptionName> filenameOption, std::optional<OptionName> directoryOption, std::optional<OptionName> dimensionOption)
    {
        if (filenameOption)
        {
            _filenameOptionString = filenameOption->longName;
            _shortFilenameOptionString = filenameOption->shortName;
        }

        if (directoryOption)
        {
            _directoryOptionString = directoryOption->longName;
            _shortDirectoryOptionString = directoryOption->shortName;
        }

        if (dimensionOption)
        {
            _dimensionOptionString = dimensionOption->longName;
            _shortDimensionOptionString = dimensionOption->shortName;
        }
    }

    utilities::CommandLineParseResult ParsedDataLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> parseErrorMessages;
        bool isFileReadable = false;

        if (!inputDataFilename.empty())
        {
            isFileReadable = utilities::IsFileReadable(GetDataFilePath());
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

            auto stream = utilities::OpenIfstream(GetDataFilePath());
            auto exampleIterator = GetAutoSupervisedExampleIterator(stream);
            while (exampleIterator.IsValid())
            {
                auto size = exampleIterator.Get().GetDataVector().PrefixLength();
                parsedDataDimension = std::max(parsedDataDimension, size);
                exampleIterator.Next();
            }
        }
        else if (dataDimension != "")
        {
            utilities::Parse(ptr, parsedDataDimension);
        }

        return parseErrorMessages;
    }
} // namespace common
} // namespace ell
