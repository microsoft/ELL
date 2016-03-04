////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CommandLineParser.cpp (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CommandLineParser.h"

// stl
#include <iostream>
#include <algorithm>
#include <sstream>

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

    CommandLineParseResult ParsedArgSet::PostProcess(const CommandLineParser& parser)
    {
        return CommandLineParseResult();
    }

    //
    // CommandLineParseResult class
    //
    CommandLineParseResult::CommandLineParseResult() : _isOK(true)
    {
    }

    CommandLineParseResult::CommandLineParseResult(bool ok) : _isOK(ok)
    {
    }

    CommandLineParseResult::CommandLineParseResult(const char* message) : _isOK(false)
    {
        _messages.emplace_back(message);
    }

    CommandLineParseResult::CommandLineParseResult(const std::string& message) : _isOK(false)
    {
        _messages.emplace_back(message);
    }

    CommandLineParseResult::CommandLineParseResult(const std::vector<std::string>& messages) : _messages(messages)
    {
        _isOK = _messages.size() == 0;
    }

    CommandLineParseResult::operator bool()
    {
        return _isOK;
    }


    //
    // ParseError class
    //
    ParseError::ParseError(const std::string& message) : _message(message)
    {
    }

    std::string ParseError::GetMessage() const
    {
        return _message;
    }

    //
    // OptionInfo class
    //
    OptionInfo::OptionInfo(std::string name, std::string shortName, std::string description, std::string defaultValue, std::function<bool(std::string)> set_value_callback) : name(name), shortName(shortName), description(description), defaultValueString(defaultValue), set_value_callbacks({ set_value_callback })
    {}

    //
    // CommandLineParser class
    //
    CommandLineParser::CommandLineParser(int argc, char* argv[]) 
    {
        SetArgs(argc, const_cast<const char**>(argv));
    }

    CommandLineParser::CommandLineParser(int argc, const char* argv[]) 
    {
        SetArgs(argc, argv);
    }

    void CommandLineParser::SetArgs(int argc, const char* argv[])
    {
        _originalArgs.clear();
        _originalArgs.insert(_originalArgs.end(), &argv[0], &argv[argc]);

        std::string exe_path = _originalArgs[0];
        size_t slashPos = exe_path.find_last_of("/\\");
        if (slashPos == std::string::npos)
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
            std::set<std::string> unset_args;
            for (const auto& opt : _options)
            {
                unset_args.insert(opt.first);
            }

            needsReparse = false;
            size_t argc = _originalArgs.size();
            for (int index = 1; index < argc; index++)
            {
                std::string arg = _originalArgs[index];
                if (arg[0] == '-') // it's an option
                {
                    std::string option;
                    if (arg[1] == '-') // long name
                    {
                        option = std::string(arg.begin() + 2, arg.end());
                    }
                    else // short name
                    {
                        std::string shortName = std::string(arg.begin() + 1, arg.end());
                        option = _shortToLongNameMap[shortName];
                    }

                    if (option == "" && arg != "--") // "--" is the special "ignore this" option --- used to put between flag arguments and the filepath
                    {
                        std::string errorMessage = "Error: unknown option " + arg;
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
                            std::string val = _originalArgs[index + 1];
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

            // Need to std::set default args here, in case one of them enables a conditional argument std::set
            needsReparse = SetDefaultArgs(unset_args) || needsReparse;
        }

        // Finally, invoke the post-parse callbacks
        bool isValid = true;
        std::vector<ParseError> parseErrors;
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

    bool CommandLineParser::SetDefaultArgs(const std::set<std::string>& unset_args)
    {
        bool needsReparse = false;
        for (std::string argName : unset_args)
        {
            // look up arg
            auto iter = _options.find(argName);
            if (iter != _options.end())
            {
                const OptionInfo& arg_info = iter->second;
                std::string default_val = arg_info.defaultValueString;
                needsReparse = SetOption(argName, default_val) || needsReparse;
            }
        }
        return needsReparse;
    }

    bool CommandLineParser::HasOption(std::string option)
    {
        return _options.find(option) != _options.end();
    }

    bool CommandLineParser::HasShortName(std::string shortName)
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
        _docEntries.emplace_back(DocumentationEntry::Type::option, info.name);

        if (info.shortName != "")
        {
            _shortToLongNameMap[info.shortName] = info.name;
        }
    }

    void CommandLineParser::AddPostParseCallback(const PostParseCallback& callback)
    {
        _postParseCallbacks.push_back(callback);
    }

    inline bool FindBestMatch(std::string str, const std::vector<std::string>& val_names, std::string& resultString)  // TODO: this std::function is not used anywhere -erase it?
    {
        bool didFindOne = false;
        for (const auto& valName : val_names)
        {
            if (valName.find(str) != std::string::npos)
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

    bool CommandLineParser::SetOption(std::string option_name, std::string option_val)
    {
        std::string oldValueString = _options[option_name].currentValueString;
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

    void CommandLineParser::AddDocumentationString(std::string str)
    {
        _docEntries.emplace_back(DocumentationEntry::Type::str, str);
    }

    std::string option_name_string(const OptionInfo& option)
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
        return std::min(maxNameLen, len);
    }

    std::string CommandLineParser::GetHelpString()
    {
        std::stringstream out;
        // Find longest option name so we can align descriptions
        size_t longest_name = 0;
        for (const auto& iter : _options)
        {
            if (iter.first == iter.second.name) // wasn't a previously-undefined option
            {
                longest_name = std::max(longest_name, optionNameHelpLength(iter.second));
            }
        }

        out << "Usage: " << _exeName << " [options]" << std::endl;
        out << std::endl;

        for (const auto& entry : _docEntries)
        {
            switch (entry.EntryType)
            {
            case DocumentationEntry::Type::option:
            {
                const OptionInfo& info = _options[entry.EntryString];
                std::string option_name = option_name_string(info);
                size_t thisOptionNameLen = optionNameHelpLength(info);
                size_t pad_len = 2 + (longest_name - thisOptionNameLen);
                std::string padding(pad_len, ' ');
                out << "\t--" << option_name << padding << info.description;
                if (info.enum_values.size() > 0)
                {
                    out << "  {";
                    std::string sep = "";
                    out << info.enum_values[0];
                    for (int index = 1; index < info.enum_values.size(); ++index)
                    {
                        out << " | " << info.enum_values[index];
                    }
                    out << "}";
                }
                out << std::endl;
            }
            break;

            case DocumentationEntry::Type::str:
                out << entry.EntryString << std::endl;
                break;
            }
        }

        return out.str();
    }

    std::string CommandLineParser::GetCurrentValuesString()
    {
        std::stringstream out;
        out << "Current parameters for " << _exeName << std::endl;

        std::set<std::string> visited_options;
        for (auto& entry : _docEntries)
        {
            if (entry.EntryType == DocumentationEntry::Type::option)
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
                    out << std::endl;
                }
                else
                {
                    out << "[" << opt.defaultValueString << "]" << std::endl;
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
                    out << std::endl;
                    out << "Unknown parameters" << std::endl;
                }
                out << "\t--" << opt.first << ": " << opt.second.currentValueString << std::endl;
                did_print = true;
            }
        }

        return out.str();
    }

    std::string CommandLineParser::GetCommandLine() const
    {
        std::stringstream out;
        out << _exeName;
        std::for_each(_originalArgs.begin() + 1, _originalArgs.end(), [&out](const std::string& s) { out << " " << s; });
        return out.str();
    }

    std::string CommandLineParser::GetOptionValue(const std::string& option)
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
