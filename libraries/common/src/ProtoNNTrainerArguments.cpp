////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainerArguments.cpp (common)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProtoNNTrainerArguments.h"

namespace ell
{
namespace common
{
    void ParsedProtoNNTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(projectedDimension,
            "projectedDimension",
            "pd",
            "The projected dimension", 5);

        parser.AddOption(numPrototypesPerLabel,
            "numPrototypesPerLabel",
            "mp",
            "The number of prototypes", 10);

        parser.AddOption(numLabels,
            "numLabels",
            "l",
            "The number of labels", 2);

        parser.AddOption(sparsityW,
            "sparsityW",
            "sw",
            "The sparsity parameter for W", 1.0);

        parser.AddOption(sparsityB,
            "sparsityB",
            "sb",
            "The sparsity parameter for B", 1.0);

        parser.AddOption(sparsityZ,
            "sparsityZ",
            "sz",
            "The sparsity parameter for Z",	1.0);

        parser.AddOption(gamma,
            "gamma",
            "g",
            "The gamma value",
            -1.0);

        parser.AddOption(lossFunction,
            "protonnLossFunction",
            "plf",
            "Choice of loss function",
            { { "L2", trainers::ProtoNNLossFunction::L2 },{ "L4", trainers::ProtoNNLossFunction::L4 } }, "L4");

        parser.AddOption(numIterations,
            "numIterations",
            "nIter",
            "Number of outer iterations",
            20);

        parser.AddOption(numInnerIterations,
            "numInnerIterations",
            "nInnerIter",
            "Number of inner iterations",
            1);
    }
}
}
