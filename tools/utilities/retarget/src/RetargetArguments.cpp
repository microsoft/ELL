////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RetargetArguments.cpp (utilities)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RetargetArguments.h"

namespace ell
{
    void ParsedRetargetArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        using namespace common;

        parser.AddOption(neuralNetworkFilename,
            "neuralNetworkFilename",
            "nn",
            "Name of the pre-trained neural network ELL map file (e.g. model1.ell) that will be used as a featurizer for a linear predictor",
            "");

        parser.AddOption(outputModelFilename,
            "outputModelFilename",
            "omf",
            "Name of the output file that will hold the saved retargeted model (e.g. retargetedModel.ell)",
            "");

        parser.AddOption(refineIterations,
            "refineIterations",
            "ri",
            "If cutting the neural network using a node id, specifies the maximum number of refinement iterations",
            1);

        parser.AddOption(targetNodeId,
            "targetNodeId",
            "tid",
            "The node id of the pre-trained neural network to use as input to the subsequent linear predictor",
            "");

        parser.AddOption(removeLastLayers,
            "removeLastLayers",
            "rem",
            "Instead of using a node id, the neural network can be cut by removing the last N layers",
            0);

        parser.AddOption(
            inputDataFilename,
            "inputDataFilename",
            "idf",
            "Path to the input dataset file",
            "");

        parser.AddOption(
            multiClass,
            "multiClass",
            "mc",
            "Indicates whether the input dataset is multi-class or binary.",
            false);

        parser.AddOption(normalize,
            "normalize",
            "n",
            "Perform sparsity-preserving normalization",
            false);

        parser.AddOption(regularization,
            "regularization",
            "r",
            "The L2 regularization parameter",
            1.0);

        parser.AddOption(desiredPrecision,
            "desiredPrecision",
            "de",
            "The desired duality gap at which to stop optimizing",
            1.0e-5);

        parser.AddOption(maxEpochs,
            "maxEpochs",
            "me",
            "The maximum number of optimizaiton epochs to run",
            1000);

        parser.AddOption(permute,
            "permute",
            "p",
            "Whether or not to randomly permute the training data before each epoch",
            true);

        parser.AddOption(randomSeedString,
            "randomSeedString",
            "seed",
            "The random seed string",
            "ABCDEFG");

        parser.AddOption(
            verbose,
            "verbose",
            "v",
            "Print diagnostic output during the execution of the tool to stdout",
            false);

        parser.AddOption(
            lossFunctionArguments.lossFunction,
            "lossFunction",
            "lf",
            "Choice of loss function",
            { { "squared", LossFunctionArguments::LossFunction::squared }, { "log", LossFunctionArguments::LossFunction::log }, {"smoothHinge", LossFunctionArguments::LossFunction::smoothHinge} },
            "log");
    }
}
