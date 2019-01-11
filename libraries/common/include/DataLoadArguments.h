////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoadArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/CommandLineParser.h>

#include <optional>
#include <string>

namespace ell
{
/// <summary> common namespace </summary>
namespace common
{
    /// <summary> A struct that holds command line parameters for loading data. </summary>
    struct DataLoadArguments
    {
        /// <summary> Accessor for the full file path. </summary>
        std::string GetDataFilePath() const;

        /// <summary> The filename for the input data file. </summary>
        std::string inputDataFilename = "";

        /// <summary> The directory for the input data file. </summary>
        std::string inputDataDirectory = "";

        /// <summary> The number of elements in an input data vector. </summary>
        std::string dataDimension = "";

        // not exposed on the command line
        size_t parsedDataDimension = 0;
    };

    struct OptionName
    {
        OptionName(std::string longName) :
            longName(longName),
            shortName(""){};
        OptionName(std::string longName, std::string shortName) :
            longName(longName),
            shortName(shortName){};

        std::string longName;
        std::string shortName;
    };

    /// <summary> A version of DataLoadArguments that adds its members to the command line parser. </summary>
    struct ParsedDataLoadArguments : public DataLoadArguments
        , public utilities::ParsedArgSet
    {
        /// <summary> Constructor with default option names. </summary>
        /// By default, the data filename option is "inputDataFilename" (with short option "idf"), and the
        /// data dimension option is "dataDimension" (with short option "dd")
        ParsedDataLoadArguments() = default;

        /// <summary> Constructor with custom option names. </summary>
        ///
        /// <param name=filenameOption> The command-line option string for the filename. </param>
        /// <param name=directoryOption> The command-line option string for the directory. </param>
        /// <param name=dimensionOption> The command-line option string for the data dimension. </param>
        ParsedDataLoadArguments(std::optional<OptionName> filenameOption, std::optional<OptionName> directoryOption, std::optional<OptionName> dimensionOption);

        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;

        /// <summary> Checks the parsed arguments. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        ///
        /// <returns> An utilities::CommandLineParseResult. </returns>
        utilities::CommandLineParseResult PostProcess(const utilities::CommandLineParser& parser) override;

    private:
        std::string _filenameOptionString = "inputDataFilename";
        std::string _shortFilenameOptionString = "idf";
        std::string _directoryOptionString = "inputDataDirectory";
        std::string _shortDirectoryOptionString = "idd";
        std::string _dimensionOptionString = "dataDimension";
        std::string _shortDimensionOptionString = "dd";
    };
} // namespace common
} // namespace ell
