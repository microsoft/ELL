////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ApplyArguments.cpp (apply)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ApplyArguments.h"

namespace emll
{
    void ParsedApplyArguments::AddArgs(utilities::CommandLineParser & parser)
    {
        parser.AddOption(
            inputMapFilename2,
            "inputMapFilename2",
            "imap2",
            "Path to the second input map file (used only in summarization mode). If provided, the second map is subtracted from the first.",
            "");

        parser.AddOption(
            summarize,
            "summarize",
            "s",
            "Aggregate and summarize map output.",
            false);
    }

    utilities::CommandLineParseResult ParsedApplyArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        std::vector<std::string> errors;
        return errors;
    }
}
