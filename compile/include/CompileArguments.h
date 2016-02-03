// CompileArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

struct CompileArguments
{
    std::string outputCodeFile;
};

struct ParsedCompileArguments : public CompileArguments, public utilities::ParsedArgSet
{
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputCodeFile,
            "outputCodeFile",
            "ocf",
            "Path to the output code file",
            "");
    }
};
