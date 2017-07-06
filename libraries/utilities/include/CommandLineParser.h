////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CommandLineParser.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary>
    /// The result returned from post-parse callback routines.
    /// A user-specified post-parse validation routine should return an instance of this class
    /// to indicate whether or not there was an error with the parameter configuration.
    /// </summary>
    class CommandLineParseResult
    {
    public:
        CommandLineParseResult() = default;

        /// <summary> Result indicating result with no message </summary>
        ///
        /// <param name="ok"> If false, this result indicates an error </param>
        CommandLineParseResult(bool ok);

        /// <summary> Result indicating an error with a message</summary>
        ///
        /// <param name="message"> The message accompanying this result </param>
        CommandLineParseResult(const char* message);

        /// <summary> Result indicating an error with a message</summary>
        ///
        /// <param name="message"> The message accompanying this result </param>
        CommandLineParseResult(const std::string& message);

        /// <summary> Result indicating a collection of zero or more errors with messages</summary>
        ///
        /// <param name="message"> The messages accompanying this result </param>
        CommandLineParseResult(const std::vector<std::string>& messages);

        /// <summary> Returns true if result is OK (there were no errors) </summary>
        operator bool() { return _isOK; }

        friend class CommandLineParser;

    private:
        std::vector<std::string> _messages;
        bool _isOK = true;
    };

    class ParsedArgSet;

    /// <summary> The command line parser class </summary>
    class CommandLineParser
    {
    public:
        /// <summary> Constructs a new CommandLineParser with the given argument strings </summary>
        ///
        /// <param name="argc"> The number of argument strings in the array of arguments </param>
        /// <param name="argv"> The array of argument strings </param>
        CommandLineParser(int argc, char* argv[]);
        CommandLineParser(int argc, const char* argv[]);

        /// <summary> Adds a new option to the command-line parser </summary>
        ///
        /// <param name="optionValue"> [out] A reference to the variable to get filled in by the parser </param>
        /// <param name="name"> The long name for the option. The option will be specified by '--' plus the long name (e.g., '--help') </param>
        /// <param name="shortName"> [optional] The short name for the option. The option will be secified by '-' plus the short name (e.g., '-h') </param>
        /// <param name="description"> The descriptive text that appears when help is requested </param>
        /// <param name="defaultValue"> The default value for the option. The optionValue argument gets set to this if no value is specified on the command line </param>
        template <typename T, typename U>
        void AddOption(T& optionValue, std::string name, std::string shortName, std::string description, const U& defaultValue, std::string emptyValueString = "true");

        /// <summary> Adds a new enumerated-value option to the command-line parser </summary>
        ///
        /// <param name="optionValue"> [out] A reference to the variable to get filled in by the parser </param>
        /// <param name="name"> The long name for the option. The option will be specified by '--' plus the long name (e.g., '--help') </param>
        /// <param name="shortName"> [optional] The short name for the option. The option will be secified by '-' plus the short name (e.g., '-h') </param>
        /// <param name="description"> The descriptive text that appears when help is requested </param>
        /// <param name="enumValues"> A list of allowed option strings and their values for this option.</param>
        /// <param name="defaultValue"> The default value for this option. The optionValue argument gets set to this if no value is specified on the command line </param>
        template <typename T>
        void AddOption(T& optionValue, std::string name, std::string shortName, std::string description, std::initializer_list<std::pair<std::string, T>> enumValues, std::string defaultValue, std::string emptyValueString = "true");

        /// <summary> Adds a new enumerated-value option to the command-line parser with a string option type </summary>
        ///
        /// <param name="optionValue"> [out] A reference to the variable to get filled in by the parser </param>
        /// <param name="name"> The long name for the option. The option will be specified by '--' plus the long name (e.g., '--help') </param>
        /// <param name="shortName"> [optional] The short name for the option. The option will be secified by '-' plus the short name (e.g., '-h') </param>
        /// <param name="description"> The descriptive text that appears when help is requested </param>
        /// <param name="enumValues"> A list of allowed option strings.</param>
        /// <param name="defaultValue"> The default value for this option. The optionValue argument gets set to this if no value is specified on the command line </param>
        void AddOption(std::string& optionValue, std::string name, std::string shortName, std::string description, std::initializer_list<std::string> enumValues, std::string defaultValue, std::string emptyValueString = "");

        /// <summary> Adds a ParsedArgSet representing a bundle of options to the commandline parser </summary>
        ///
        /// <param name="options"> The ParsedArgSet containing the options </param>
        void AddOptionSet(ParsedArgSet& options);

        /// <summary> Adds a string that gets printed out help is requested </summary>
        ///
        /// <param name="docString"> The string to be printed </param>
        virtual void AddDocumentationString(std::string docString);

        using PostParseCallback = std::function<CommandLineParseResult(CommandLineParser&)>;

        /// <summary> Adds a callback function that gets invoked after Parse() is called </summary>
        ///
        /// <param name="callback"> The callback function to invoke after parsing is done </param>
        void AddPostParseCallback(const PostParseCallback& callback);

        /// <summary> Parses the commandline. Call this after setting up the options with AddOption and/or AddOptionSet </summary>
        void Parse();

        /// <summary> Returns the help string </summary>
        ///
        /// <returns> The help string </returns>
        std::string GetHelpString();

        /// <summary> Returns a string describing the current values assigned to the options </summary>
        ///
        /// <returns> A string describing the current values assigned to the options </returns>
        std::string GetCurrentValuesString();

        /// <summary> Returns a copy of the original options passed to the command line </summary>
        std::string GetCommandLine() const;

        /// <summary> Returns the value of a given option (as a string) </summary>
        std::string GetOptionValue(const std::string& option);

        /// <summary> Returns true if the given option has been registered </summary>
        ///
        /// <param name="option"> The long name for the option </param>
        /// <returns> true if the given option has been registered </returns>
        bool HasOption(std::string option);

        /// <summary> Returns true if the given short name has been registered </summary>
        ///
        /// <param name="shortName"> The short name </param>
        /// <returns> true if the given short name has been registered </returns>
        bool HasShortName(std::string shortName);

    private:
        CommandLineParser(const CommandLineParser&) = delete;

        void SetArgs(int argc, const char* argv[]);

        template <typename T>
        static bool ParseVal(std::string str, T& result);

        template <typename T>
        static bool ParseVal(std::string str, std::vector<std::pair<std::string, T>> val_names, T& result, std::string& resultString);

        template <typename T>
        static std::string ToString(const T& val);

        struct DocumentationEntry
        {
            enum Type
            {
                option,
                str
            };
            Type EntryType;
            std::string EntryString; // option name for option, docstring for std::string

            DocumentationEntry(Type t, std::string str)
                : EntryType(t), EntryString(str) {}
        };

        struct OptionInfo
        {
            std::string name;
            std::string shortName;
            std::string description;
            std::string defaultValueString;
            std::string emptyValueString; // value to use if no value follows the option
            std::string currentValueString;
            std::vector<std::string> enumValues;

            std::vector<std::function<bool(std::string)>> set_value_callbacks; // callback returns "true" if value was successfully std::set, otherwise "false"
            std::vector<std::function<bool(std::string)>> didSetValueCallbacks; // callback returns "true" if value was successfully std::set, otherwise "false"

            OptionInfo()
            {
            }

            OptionInfo(std::string name, std::string shortName, std::string description, std::string defaultValue, std::string emptyValueString, std::function<bool(std::string)> set_value_callback);

            std::string optionNameString() const;
            size_t optionNameHelpLength() const;
        };

        std::vector<std::string> _originalArgs;
        std::string _exeName;
        std::vector<std::string> _positionalArgs; // these are filename-type args at the end, currently unused
        std::map<std::string, std::string> _shortToLongNameMap;
        std::map<std::string, OptionInfo> _options;
        std::vector<DocumentationEntry> _docEntries;
        std::vector<PostParseCallback> _postParseCallbacks;

        void AddOption(const OptionInfo& info);
        virtual bool SetOption(std::string option_name); // returns true if we need to reparse
        virtual bool SetOption(std::string option_name, std::string option_val); // returns true if we need to reparse
        bool SetDefaultArgs(const std::set<std::string>& unset_args); // returns true if we need to reparse
    };

    /// <summary> A mixin class to make parameter structs be parseable. </summary>
    class ParsedArgSet
    {
    public:
        ParsedArgSet() = default;
        virtual ~ParsedArgSet() = default;

        /// <summary>
        /// Adds the arguments stored in this arg set to the command-line parser.
        /// Subclasses should override this method and add their arguments to the parser.
        /// </summary>
        ///
        /// <param name="parser"> The command-line parser to register arguments with </param>
        virtual void AddArgs(CommandLineParser& parser);

        /// <summary> Called by the parser after parsing has been done. </summary>
        ///
        /// <param name="parser"> The command-line parser that parsed the arguments </param>
        virtual CommandLineParseResult PostProcess(const CommandLineParser& parser);
    };

    // Exceptions thrown by CommandLineParser:
    //
    /// <summary> The base class for all command-line parser-related exceptions. </summary>
    class CommandLineParserException : public std::runtime_error
    {
    public:
        CommandLineParserException(const char* message)
            : std::runtime_error(message){};
    };

    /// <summary> An object containing a parsing error message from the command-line parser. </summary>
    class ParseError
    {
    public:
        /// <summary> Gets the error message for this error </summary>
        ///
        /// <returns> The error message for this error </returns>
        std::string GetMessage() const { return _message; }

        /// <summary> Constructor for ParseError class. Used by CommandLineParserErrorException. </summary>
        ParseError(const std::string& message);

    private:
        std::string _message;
    };

    /// <summary> An exception representing an error during parsing. </summary>
    class CommandLineParserErrorException : CommandLineParserException
    {
    public:
        /// <summary> Returns list of parse errors that occured during parsing </summary>
        ///
        /// <returns> A list of parse errors that occured during parsing </returns>
        const std::vector<ParseError>& GetParseErrors() const { return _errors; }

        /// <summary> Constructor. Called by command-line parser </summary>
        CommandLineParserErrorException(const char* message)
            : CommandLineParserException(message) {}
        CommandLineParserErrorException(const char* message, std::vector<ParseError> errors)
            : CommandLineParserException(message), _errors(errors) {}
    private:
        std::vector<ParseError> _errors;
    };

    /// <summary> An exception indicating the "print help and exit" option was selected. </summary>
    class CommandLineParserPrintHelpException : public CommandLineParserException
    {
    public:
        /// <summary> Returns help text describing the valid command-line options </summary>
        ///
        /// <returns> Help text describing the valid command-line options </returns>
        std::string GetHelpText() const { return _helpText; }

        /// <summary> Constructor. Called by command-line parser </summary>
        CommandLineParserPrintHelpException(std::string helpText)
            : CommandLineParserException(""), _helpText(helpText) {}

    private:
        std::string _helpText;
    };

    /// <summary> An exception indicating an error with the supplied command-line parameters. </summary>
    class CommandLineParserInvalidOptionsException : public CommandLineParserException
    {
    public:
        CommandLineParserInvalidOptionsException(const char* what)
            : CommandLineParserException(what) {}
    };
}
}

#include "../tcc/CommandLineParser.tcc"
