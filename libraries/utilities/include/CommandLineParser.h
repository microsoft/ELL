// CommandLineParser.h

// cjacobs: Next steps to do:
// * Add policy field for options (required, std::set only once, std::set at least once, last wins, collect all, ...)
// * Add more flexible constraints (using a callback?) for non-enum parameters
// * have one "master" parser that has subordinate parsers to handle parameters for different modules (e.g., tree learner vs. booster vs. global optimizer)
// * have conditional parsers whose std::set of known options depends on some other option --- maybe via polymorphism (so, have a gradient_descent parser and 
//   a sdca parser, both of which are subclasses of a global_descent parser, the choice of which to use is conditional on the gd_alg option
// * tokenize the input stream based on whitespace, then split on entries starting with '-' --- the first item will be the option, followed by zero or more values
//   in this case, we maybe need to use the special '--' option to specify positional / unnamed arguments (and/or have them appear first)
// * (optionally?) accept /arg instead of -arg or --arg

#pragma once

// stl
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <string>
#include <stdexcept>

namespace utilities
{
    /// TODO: document
    ///
    struct OptionInfo
    {
        std::string name;
        std::string shortName;
        std::string description;
        std::string defaultValueString; // for printing
        std::string currentValueString;
        std::vector<std::string> enum_values; // for enumerated values // TODO: make this into a more general constraint mechanism (?)

        std::vector<std::function<bool(std::string)>> set_value_callbacks; // callback returns "true" if value was successfully std::set, otherwise "false"
        std::vector<std::function<bool(std::string)>> didSetValueCallbacks; // callback returns "true" if value was successfully std::set, otherwise "false"

        // TODO: Add "policy" member (required, set_once, last_wins, etc.)

        OptionInfo()
        {}

        /// TODO: document
        ///
        OptionInfo(std::string name, std::string shortName, std::string description, std::string defaultValue, std::function<bool(std::string)> set_value_callback);

        // TODO: either have get_help_string() or print_help() methods for help and current value
    };


    class ParseResult
    {
    public:
        ParseResult(); // No error
        ParseResult(bool ok); // Error (if ok == false), no message
        ParseResult(const char *message); // Error
        ParseResult(const std::string& message); // Error
        ParseResult(const std::vector<std::string>& messages); // list of errors (or success, if empty)
        operator bool();

        friend class CommandLineParser;

    private:
        std::vector<std::string> _messages;
        bool _isOK;
    };

    class ParseError
    {
    public:
        ParseError(const std::string& message);
        std::string GetMessage() const;

    private:
        std::string _message;
    };

    // format of argv: Main.exe [options]
    // where options are of the form "-<std::string> <option>" where the <option> part is mandatory (defaulting to 'true')
    // options have two names, the short name is used with a single hyphen, and the long name with two
    // e.g., "-s true" and "--serial_mode true" can mean the same thing
    // options are queried by the long name
    // short name is optional
    // args are just strings at the end
    // ex of valid commandlines:
    // myexe.exe foo.tsv
    // myexe.exe foo.tsv bar.tsv
    // myexe.exe -t 8 -x blah foo.tsv bar.tsv
    class ParsedArgSet;
    class CommandLineParser
    {
    public:
    
        /// Constructor, takes arg list
        ///
        CommandLineParser(int argc, char**argv);
        
        /// AddOption adds a new option to the commandline parser
        ///
        template <typename T, typename U>
        void AddOption(T& option, std::string name, std::string shortName, std::string description, const U& defaultValue);

        /// AddOptionSet adds a ParsedArgSet to the commandline parser
        ///
        void AddOptionSet(ParsedArgSet& options);

        /// Adds a std::string that gets printed out when pring_usage() is called
        ///
        virtual void AddDocumentationString(std::string str);

        /// Parses the commandline. Call this after setting up the options with AddOption
        ///
        void Parse();

        /// TODO: document
        ///
        std::string GetHelpString();

        /// TODO: document
        ///
        std::string GetCurrentValuesString();

        std::string GetCommandLine() const;

        std::string GetOptionValue(const std::string& option);

        /// TODO: document
        ///
        bool HasOption(std::string option);

        bool HasShortName(std::string shortName);

        /// Adds a callback std::function that gets invoked after Parse() is called
        using PostParseCallback = std::function<ParseResult(CommandLineParser&)>;
        void AddPostParseCallback(const PostParseCallback& callback);

    protected:

        /// TODO: document
        ///
        CommandLineParser(const CommandLineParser&) = delete;

        /// TODO: document
        ///
        void SetArgs(int argc, char** argv);

        /// TODO: document
        ///
        template <typename T>
        static bool ParseVal(std::string str, T& result);

        /// TODO: document
        ///
        template <typename T>
        static bool ParseVal(std::string str, std::vector<std::pair<std::string, T>> val_names, T& result, std::string& resultString);

        /// TODO: document
        ///
        template <typename T>
        static std::string ToString(const T& val);

        struct DocumentationEntry
        {
            enum Type { option, str };
            Type EntryType;
            std::string EntryString; // option name for option, docstring for std::string

            DocumentationEntry(Type t, std::string str) : EntryType(t), EntryString(str) {}
        };

        std::vector<std::string> _originalArgs;
        std::string _exeName;
        std::vector<std::string> _positionalArgs; // these are filename-type args at the end, currently unused
        std::map<std::string, std::string> _shortToLongNameMap;
        std::map<std::string, OptionInfo> _options;
        std::vector<DocumentationEntry> _docEntries;
        std::vector<PostParseCallback> _postParseCallbacks;

        void AddOption(const OptionInfo& info);
        virtual bool SetOption(std::string option_name, std::string option_val); // returns true if we need to reparse
        bool SetDefaultArgs(const std::set<std::string>& unset_args); // returns true if we need to reparse
    };

    /// ParsedArgSet class
    ///
    class ParsedArgSet
    {
    public:
        ParsedArgSet();

        virtual void AddArgs(CommandLineParser& parser);
        virtual ParseResult PostProcess(const CommandLineParser& parser);
    };

    /// Exceptions thrown by CommandLineParser: 
    ///
    class CommandLineParserException : public std::runtime_error
    {
    public:
        CommandLineParserException(const char* message) : std::runtime_error(message) {};
    };

    class CommandLineParserErrorException : CommandLineParserException
    {
    public:
        CommandLineParserErrorException(const char* message) : CommandLineParserException(message){}
        CommandLineParserErrorException(const char* message, std::vector<ParseError> errors) : CommandLineParserException(message), _errors(errors) {}
        const std::vector<ParseError>& GetParseErrors() const { return _errors; }

    private:
        std::vector<ParseError> _errors;
    };

    class CommandLineParserPrintHelpException : public CommandLineParserException
    {
    public:
        CommandLineParserPrintHelpException(std::string helpText) : CommandLineParserException(""), _helpText(helpText) {}
        std::string GetHelpText() const { return _helpText; }

    private:
        std::string _helpText;
    };


    class CommandLineParserInvalidOptionsException : public CommandLineParserException
    {
    public:
        CommandLineParserInvalidOptionsException(const char* what) : CommandLineParserException(what) {}
    };
}

#include "../tcc/CommandLineParser.tcc"
