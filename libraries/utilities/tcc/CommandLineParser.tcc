// CommandLineParser.tcc

#include <stdexcept>

#include <string>
using std::string;
using std::pair;

#include <sstream>
using std::stringstream;

namespace utilities
{
    // format of argv: Main.exe [options]
    // where options are of the form "-<string> <option>" where the <option> part is mandatory (defaulting to 'true')
    // options have two names, the short name is used with a single hyphen, and the long name with two
    // e.g., "-s true" and "--serial_mode true" can mean the same thing
    // options are queried by the long name
    // short name is optional
    // args are just strings at the end
    // ex of valid commandlines:
    // myexe.exe foo.tsv
    // myexe.exe foo.tsv bar.tsv
    // myexe.exe -t 8 -x blah foo.tsv bar.tsv
    template <typename T, typename U>
    void CommandLineParser::AddOption(T& option, string name, string shortName, string description, const U& defaultValue)
    {
        auto callback = [&option, this](string option_val)
        {
            bool did_parse = ParseVal<T>(option_val, option);
            return did_parse;
        };

        OptionInfo info(name, shortName, description, ToString(defaultValue), callback);
        AddOption(info);
    }

    template <typename T>
    bool CommandLineParser::ParseVal(string str, T& result)
    {
        stringstream ss(str);
        ss >> result;
        return true;
    }

    template <typename T>
    bool CommandLineParser::ParseVal(string str, vector<pair<string, T>> val_names, T& result, string& result_string)
    {
        bool did_find_one = false;
        for (const auto& val_name_pair : val_names)
        {
            if (val_name_pair.first.find(str) == 0)
            {
                if (did_find_one)
                {
                    return false;
                }

                result_string = val_name_pair.first;
                result = val_name_pair.second;
                did_find_one = true;
            }
        }

        return did_find_one;
    }

    template <typename T>
    string CommandLineParser::ToString(const T& val)
    {
        stringstream ss;
        ss << val;
        return ss.str();
    }


    // bool specialization
    template <>
    inline bool CommandLineParser::ParseVal<bool>(string val, bool& result)
    {
        result = (val[0] == 't');
        return true;
    }

    template<>
    inline string CommandLineParser::ToString<bool>(const bool& val)
    {
        return val ? "true" : "false";
    }
}
