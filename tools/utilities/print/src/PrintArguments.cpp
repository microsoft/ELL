////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintArguments.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"

// utilities
#include "Files.h"

namespace ell
{
void ParsedPrintArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(outputFilename, "outputFilename", "of", "Path to the output file", "");
    parser.AddOption(outputFormat, "outputFormat", "fmt", "What output format to generate [text|dgml|dot] (default text)", "text");
	parser.AddOption(refine, "refineIterations", "ri", "If not 0, the model is refined using the specified the number of refinement iterations", 0);
    parser.AddOption(includeNodeId, "includeNodeId", "incid", "Include the node id in the print", false);
}

utilities::CommandLineParseResult ParsedPrintArguments::PostProcess(const utilities::CommandLineParser& parser)
{
    if (outputFilename == "null")
    {
        outputStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
    }
    else if (outputFilename == "")
    {
        outputStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
    }
    else // treat argument as filename
    {
        outputStream = utilities::OutputStreamImpostor(outputFilename);
    }

    std::vector<std::string> parseErrorMessages;
    return parseErrorMessages;
}
}
