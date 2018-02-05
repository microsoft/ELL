////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareArguments.cpp (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompareArguments.h"

namespace ell
{
void ParsedCompareArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddDocumentationString("Input options");
    parser.AddOption(inputMapFile, "inputMapFile", "imap", "Path to the input *.map file", "");
    parser.AddOption(inputConverter, "inputConverter", "ic", "Path to python script for converting the input to valid input for the map", "");

    parser.AddDocumentationString("Output options");
    parser.AddOption(outputDirectory, "outputDirectory", "od", "Location of output files (default cwd)", "");
    parser.AddOption(writeReport, "report", "", "Generate markdown report", true);
    parser.AddOption(writeGraph, "graph", "", "Write DGML graph", true);
    parser.AddOption(writePrediction, "pred", "", "Write prediction to report", true);
}
}
