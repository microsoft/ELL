////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CommandLineParser.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"
#include "StringUtil.h"

// stl
#include <sstream>

namespace ell
{
namespace utilities
{
    // format of argv: Main.exe [options]
    // where options are of the form "-<std::string> <option>" where the <option> part is mandatory (defaulting to 'true')
    // options have two names, the short name is used with a single hyphen, and the long name with two
    // e.g., "-s true" and "--serial_mode true" can mean the same thing
    // options are queried by the long name
    // short name is optional
    // args are just strings at the end
    // example of valid commandlines:
    // myexe.exe file1.tsv
    // myexe.exe file1.tsv file2.tsv
    // myexe.exe -t 8 -x someString file1.tsv file2.tsv
    template <typename T, typename U>
    void CommandLineParser::AddOption(T& option, std::string name, std::string shortName, std::string description, const U& defaultValue, std::string emptyValueString)
    {
        auto callback = [&option](std::string optionVal) {
            bool didParse = ParseVal<T>(optionVal, option);
            return didParse;
        };

        OptionInfo info(name, shortName, description, ToString(defaultValue), emptyValueString, callback);
        AddOption(info);
    }

    template <typename T>
    void CommandLineParser::AddOption(T& option, std::string name, std::string shortName, std::string description, std::initializer_list<std::pair<std::string, T>> enumValues, std::string defaultValue, std::string emptyValueString)
    {
        // transform initializer list into useful things that will stick around
        std::vector<std::string> valueNameStrings;
        std::vector<std::pair<std::string, T>> valueNamesTable;
        for (auto v : enumValues)
        {
            valueNameStrings.push_back(v.first);
            valueNamesTable.push_back(v);
        }

        auto callback = [&option, this, name, valueNamesTable](std::string optionVal) {
            std::string optionString;
            bool didParse = ParseVal<T>(optionVal, valueNamesTable, option, optionString);
            if (didParse)
            {
                _options[name].currentValueString = optionString;
                return true;
            }
            else
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "Could not parse value for option " + name);
                return false;
            }
        };

        OptionInfo info(name, shortName, description, defaultValue, emptyValueString, callback);
        info.enumValues = valueNameStrings;
        AddOption(info);
    }

    template <typename T>
    bool CommandLineParser::ParseVal(std::string str, T& result)
    {
        std::stringstream ss(str);
        ss >> result;
        return true;
    }

    template <typename T>
    bool CommandLineParser::ParseVal(std::string str, std::vector<std::pair<std::string, T>> valNames, T& result, std::string& resultString)
    {
        bool foundPartialMatch = false;
        for (const auto& valNamePair : valNames)
        {
            // Exact match
            if (valNamePair.first == str)
            {
                resultString = valNamePair.first;
                result = valNamePair.second;
                return true;
            }

            // Partial match
            if (valNamePair.first.find(str) == 0)
            {
                // More than one partial match -- fail
                if (foundPartialMatch)
                {
                    return false;
                }
                resultString = valNamePair.first;
                result = valNamePair.second;
                foundPartialMatch = true;
            }
        }

        return foundPartialMatch;
    }

    template <typename T>
    std::string CommandLineParser::ToString(const T& val)
    {
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    // bool specialization
    template <>
    inline bool CommandLineParser::ParseVal<bool>(std::string val, bool& result)
    {
        auto lowerval = ToLowercase(val);
        result = (val == "true" || val == "t");
        return true;
    }

    template <>
    inline std::string CommandLineParser::ToString<bool>(const bool& val)
    {
        return val ? "true" : "false";
    }
}
}
