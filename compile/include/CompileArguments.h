// CompileArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::ParsedArgSet;
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

struct CompileArguments
{
    string outputCodeFile;
};

struct ParsedCompileArguments : public CompileArguments, public ParsedArgSet
{
    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            outputCodeFile,
            "outputCodeFile",
            "ocf",
            "Path to the output code file",
            "");
    }
};
