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
            "The projected dimension. Decreasing pd will lead to smaller model size but decreasing it beyond a point can lead to loss in accuracy", 5);

        parser.AddOption(numPrototypesPerLabel,
            "numPrototypesPerLabel",
            "mp",
            "The number of prototypes. Increasing mp should lead to bigger model size and higher accuracy, up to a point. Typically, we want to have 10 prototypes per class", 10);

        parser.AddOption(numLabels,
            "numLabels",
            "l",
            "The number of labels", 2);

        parser.AddOption(sparsityW,
            "sparsityW",
            "sw",
            "The sparsity parameter for W, i.e. fraction of non-zeros in the the projection matrix W. Increasing sw leads to increase in model size and accuracy, provided there is no overfitting. So for low-d problems, we can leave sw to be 1 and for high-d problems it should be much smaller", 1.0);

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
