////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelGenerateArguments.cpp (utilities)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelGenerateArguments.h"

namespace ell
{
void ParsedModelGenerateArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(
        outputType,
        "outputType",
        "o",
        "Choice of output type: model, map",
        { { "model", OutputType::model }, { "map", OutputType::map } },
        "model");

    parser.AddOption(
        outputPath,
        "outputPath",
        "p",
        "The output path",
        "");
}
}
