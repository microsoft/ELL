/// CommandLineParser.cpp

#include "CommandLineParser.h"

#include <iostream>
using std::endl;

#include <algorithm>
using std::min;
using std::max;
using std::for_each;

namespace utilities
{
    //
    // ParsedArgSet class
    //
    ParsedArgSet::ParsedArgSet()
    {
    }

    void ParsedArgSet::AddArgs(CommandLineParser& parser)
    {
    }

    ParseResult ParsedArgSet::PostProcess(const CommandLineParser& parser)
    {
        return ParseResult();
    }

    //
    // ParseResult class
    //
    ParseResult::ParseResult() : _isOK(true)
    {
    }

    ParseResult::ParseResult(bool ok) : _isOK(ok)
    {
    }

    ParseResult::ParseResult(const char* message) : _isOK(false)
    {
        _messages.emplace_back(message);
    }

    ParseResult::ParseResult(const string& message) : _isOK(false)
    {
        _messages.emplace_back(message);
    }

    ParseResult::ParseResult(const vector<string>& messages) : _messages(messages)
    {
        _isOK = _messages.size() == 0;
    }

    ParseResult::operator bool()
    {
        return _isOK;
    }


    //
    // ParseError class
    //
    ParseError::ParseError(const string& message) : _message(message)
    {
    }

    string ParseError::GetMessage() const
    {
        return _message;
    }

    //
    // OptionInfo class
    //
    OptionInfo::OptionInfo(string name, string shortName, string description, string defaultValue, function<bool(string)> set_value_callback) : name(name), shortName(shortName), description(description), defaultValueString(defaultValue), set_value_callbacks({ set_value_callback })
    {}

    //
    // CommandLineParser class
    //
    CommandLineParser::CommandLineParser(int argc, char**argv) 
    {
        SetArgs(argc, argv);
    }

    void CommandLineParser::SetArgs(int argc, char** argv)
    {
        _originalArgs.clear();
        _originalArgs.insert(_originalArgs.end(), &argv[0], &argv[argc]);

        string exe_path = _originalArgs[0];
        size_t slashPos = exe_path.find_last_of("/\\");
        if (slashPos == string::npos)
        {
            _exeName = exe_path;
        }
        else
        {
            _exeName = exe_path.substr(slashPos + 1);
        }
    }

    void CommandLineParser::Parse()
    {
        // TODO: should probably throw an exception here ?
        if (_originalArgs.size() == 0)
            return;

        bool printHelpAndExit = false;
        if (!HasOption("help"))
        {
            if (HasShortName("h"))
            {
                AddOption(printHelpAndExit, "help", "", "Print help and exit", false);
            }
            else
            {
                AddOption(printHelpAndExit, "help", "h", "Print help and exit", false);
            }
        }

        // While we're parsing the arguments, we may add new conditional options. If we do so, we need
        // to reparse the inputs in case some earlier commandline option referred to one of these new
        // options. So we repeatedly parse the command line text until we haven't added any new conditional options.

        bool needsReparse = true;
        while (needsReparse)
        {
            set<string> unset_args;
            for (const auto& opt : _options)
            {
                unset_args.insert(opt.first);
            }

            needsReparse = false;
            size_t argc = _originalArgs.size();
            for (int index = 1; index < argc; index++)
            {
                string arg = _originalArgs[index];
                if (arg[0] == '-') // it's an option
                {
                    string option;
                    if (arg[1] == '-') // long name
                    {
                        option = string(arg.begin() + 2, arg.end());
                    }
                    else // short name
                    {
                        string shortName = string(arg.begin() + 1, arg.end());
                        option = _shortToLongNameMap[shortName];
                    }

                    if (option == "" && arg != "--") // "--" is the special "ignore this" option --- used to put between flag arguments and the filepath
                    {
                        string errorMessage = "Error: unknown option " + arg;
                        throw CommandLineParserErrorException("Unknown option", {errorMessage});
                        if (index < argc - 1 && _originalArgs[index + 1][0] != '-')  // skip the Next value as well, unless it's an option
                        {
                            index++;
                        }
                    }

                    if (option != "")
                    {
                        unset_args.erase(option);
                        if (index < argc - 1)
                        {
                            string val = _originalArgs[index + 1];
                            if (val[0] == '-')
                            {
                                needsReparse = SetOption(option, "true") || needsReparse;
                            }
                            else
                            {
                                needsReparse = SetOption(option, val) || needsReparse;
                                index++;
                            }
                        }
                        else // this is the last thing on the line --- assume it's a shortcut for --option true
                        {
                            needsReparse = SetOption(option, "true") || needsReparse;
                        }
                    }
                }
                else
                {
                    _positionalArgs.push_back(_originalArgs[index]);
                }
            }

            // Need to set default args here, in case one of them enables a conditional argument set
            needsReparse = SetDefaultArgs(unset_args) || needsReparse;
        }

        // Finally, invoke the post-parse callbacks
        bool isValid = true;
        vector<ParseError> parseErrors;
        for (const auto& callback : _postParseCallbacks)
        {
            auto callbackResult = callback(*this);
            if (!callbackResult) // callbackResult is an error
            {
                isValid = false;
                for (auto message : callbackResult._messages)
                {
                    parseErrors.emplace_back(message);
                }
            }
        }

        if (printHelpAndExit)
        {
            throw CommandLineParserPrintHelpException(GetHelpString());
        }

        if (!isValid)
        {
            throw CommandLineParserErrorException("Error in parse callback", parseErrors);
        }
    }

    bool CommandLineParser::SetDefaultArgs(const set<string>& unset_args)
    {
        bool needsReparse = false;
        for (string argName : unset_args)
        {
            // look up arg
            auto iter = _options.find(argName);
            if (iter != _options.end())
            {
                const OptionInfo& arg_info = iter->second;
                string default_val = arg_info.defaultValueString;
                needsReparse = SetOption(argName, default_val) || needsReparse;
            }
        }
        return needsReparse;
    }

    bool CommandLineParser::HasOption(string option)
    {
        return _options.find(option) != _options.end();
    }

    bool CommandLineParser::HasShortName(string shortName)
    {
        return _shortToLongNameMap.find(shortName) != _shortToLongNameMap.end();
    }

    void CommandLineParser::AddOption(const OptionInfo& info)
    {
        if (_options.find(info.name) != _options.end())
        {
            throw CommandLineParserInvalidOptionsException("Error: adding same option more than once");
        }

        if (_shortToLongNameMap.find(info.shortName) != _shortToLongNameMap.end())
        {
            throw CommandLineParserInvalidOptionsException("Error: adding same short name more than once");
        }

        _options[info.name] = info;
        _docEntries.emplace_back(DocumentationEntry::type::option, info.name);

        if (info.shortName != "")
        {
            _shortToLongNameMap[info.shortName] = info.name;
        }
    }

    void CommandLineParser::AddPostParseCallback(const PostParseCallback& callback)
    {
        _postParseCallbacks.push_back(callback);
    }

    inline bool FindBestMatch(string str, const vector<string>& val_names, string& resultString)  // TODO: this function is not used anywhere -erase it?
    {
        bool didFindOne = false;
        for (const auto& valName : val_names)
        {
            if (valName.find(str) != string::npos)
            {
                if (didFindOne)
                {
                    return false;
                }

                resultString = valName;
                didFindOne = true;
            }
        }

        return didFindOne;
    }

    bool CommandLineParser::SetOption(string option_name, string option_val)
    {
        string oldValueString = _options[option_name].currentValueString;
        _options[option_name].currentValueString = option_val;

        bool ok = true;
        bool didEnableMoreParams = false;
        for (auto setValueCb : _options[option_name].set_value_callbacks)
        {
            ok = ok && setValueCb(option_val);
        }

        if (ok)
        {
            auto iter = _options[option_name].didSetValueCallbacks.begin();
            while (iter != _options[option_name].didSetValueCallbacks.end())
            {
                auto& didSetValueCb = *iter;
                bool did_set = didSetValueCb(_options[option_name].currentValueString);
                if (did_set)
                {
                    didEnableMoreParams = true;
                    // remove from list once it's called
                    iter = _options[option_name].didSetValueCallbacks.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }

        }
        else
        {
            _options[option_name].currentValueString = oldValueString;
        }

        return didEnableMoreParams;
    }

    void CommandLineParser::AddOptionSet(ParsedArgSet& options)
    {
        AddPostParseCallback([this, &options](CommandLineParser& p) { return options.PostProcess(p);});
        options.AddArgs(*this);
    }

    void CommandLineParser::AddDocumentationString(string str)
    {
        _docEntries.emplace_back(DocumentationEntry::type::str, str);
    }

    string option_name_string(const OptionInfo& option)
    {
        if (option.shortName == "")
        {
            return option.name + " [" + option.defaultValueString + "]";
        }
        else
        {
            return option.name + " (-" + option.shortName + ") [" + option.defaultValueString + "]";
        }
    }

    size_t optionNameHelpLength(const OptionInfo& option)
    {
        size_t len = option.name.size() + 2;
        if (option.shortName != "")
        {
            len += (option.shortName.size() + 4);
        }

        len += option.defaultValueString.size() + 3; // 3 for " [" + "]" at begin/end

        const size_t maxNameLen = 32;
        return min(maxNameLen, len);
    }

    string CommandLineParser::GetHelpString()
    {
        stringstream out;
        // Find longest option name so we can align descriptions
        size_t longest_name = 0;
        for (const auto& iter : _options)
        {
            if (iter.first == iter.second.name) // wasn't a previously-undefined option
            {
                longest_name = max(longest_name, optionNameHelpLength(iter.second));
            }
        }

        out << "Usage: " << _exeName << " [options]" << endl;
        out << endl;

        for (const auto& entry : _docEntries)
        {
            switch (entry.EntryType)
            {
            case DocumentationEntry::type::option:
            {
                const OptionInfo& info = _options[entry.EntryString];
                string option_name = option_name_string(info);
                size_t thisOptionNameLen = optionNameHelpLength(info);
                size_t pad_len = 2 + (longest_name - thisOptionNameLen);
                string padding(pad_len, ' ');
                out << "\t--" << option_name << padding << info.description;
                if (info.enum_values.size() > 0)
                {
                    out << "  {";
                    string sep = "";
                    out << info.enum_values[0];
                    for (int index = 1; index < info.enum_values.size(); ++index)
                    {
                        out << " | " << info.enum_values[index];
                    }
                    out << "}";
                }
                out << endl;
            }
            break;

            case DocumentationEntry::type::str:
                out << entry.EntryString << endl;
                break;
            }
        }

        return out.str();
    }

    string CommandLineParser::GetCurrentValuesString()
    {
        stringstream out;
        out << "Current parameters for " << _exeName << endl;

        set<string> visited_options;
        for (auto& entry : _docEntries)
        {
            if (entry.EntryType == DocumentationEntry::type::option)
            {
                const auto& opt = _options[entry.EntryString];
                visited_options.insert(opt.name);
                out << "\t--" << opt.name << ": ";
                if (opt.currentValueString != "")
                {
                    out << opt.currentValueString;
                    if (opt.currentValueString == opt.defaultValueString)
                    {
                        out << " (default)";
                    }
                    out << endl;
                }
                else
                {
                    out << "[" << opt.defaultValueString << "]" << endl;
                }
            }
        }

        bool did_print = false;
        for (auto& opt : _options)
        {
            if (visited_options.find(opt.first) == visited_options.end())
            {
                if (!did_print)
                {
                    out << endl;
                    out << "Unknown parameters" << endl;
                }
                out << "\t--" << opt.first << ": " << opt.second.currentValueString << endl;
                did_print = true;
            }
        }

        return out.str();
    }

    string CommandLineParser::GetCommandLine() const
    {
        stringstream out;
        out << _exeName;
        for_each(_originalArgs.begin() + 1, _originalArgs.end(), [&out](const string& s) { out << " " << s; });
        return out.str();
    }

    string CommandLineParser::GetOptionValue(const string& option)
    {
        auto it = _options.find(option);
        if (it != _options.end())
        {
            return it->second.currentValueString;
        }
        else
        {
            return "";
        }
    }
}
