// CommandLineArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::ParsedArgSet;
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

struct CommandLineArguments
{
    string mapFile;
    string codeFile;
};

struct ParsedCommandLineArguments : public CommandLineArguments, public ParsedArgSet
{
    ParsedCommandLineArguments(CommandLineParser& parser) : ParsedArgSet(parser)
    {
        AddArgs(parser);
    }

    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(mapFile, "mapFile", "lf", "Path to the input file that contains the layer information", "");
        parser.AddOption(codeFile, "codeFile", "cf", "Path to the output code file", "");
    }
};