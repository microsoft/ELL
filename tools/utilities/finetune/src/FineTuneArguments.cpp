////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FineTuneArguments.cpp (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FineTuneArguments.h"

#include <utilities/include/Exception.h>

#include <iostream>

using namespace ell;

FineTuneOptimizationParameters FineTuneArguments::GetOptimizerParameters() const
{
    optimization::SDCAOptimizerParameters params;
    params.regularizationParameter = l2Regularization;
    params.permuteData = permute;

    return { params, l1Regularization, maxEpochs, desiredPrecision, normalizeInputs, normalizeOutputs, randomSeed };
}

utilities::OutputStreamImpostor FineTuneArguments::GetReportStream() const
{
    if (reportFilename.empty())
    {
        return utilities::OutputStreamImpostor(std::cout);
    }
    else
    {
        return utilities::OutputStreamImpostor(reportFilename);
    }
}
void InitArguments(utilities::CommandLineParser& parser, FineTuneArguments& args)
{
    using namespace common;

    parser.AddOptionSet(args.mapLoadArguments);
    parser.AddOptionSet(args.mapSaveArguments);
    parser.AddOptionSet(args.trainDataArguments);
    parser.AddOptionSet(args.testDataArguments);

    parser.AddOption(args.targetPortElements,
                     "targetPortElements",
                     "tpe",
                     "The port elements of the pre-trained model to use as input to the subsequent linear predictor "
                     "e.g. \"1115.output\" to use the full output from Node 1115",
                     "");

    parser.AddOption(args.print, "print", "", "Print the model", false);

    parser.AddOption(args.multiClass,
                     "multiClass",
                     "mc",
                     "Indicates whether the input dataset is multi-class or binary.",
                     true);

    parser.AddOption(args.maxTrainingRows,
                     "maxTrainingRows",
                     "",
                     "Maximum number of rows from dataset to use for training (0 = 'all')",
                     0);

    parser.AddOption(args.maxTestingRows,
                     "maxTestingRows",
                     "",
                     "Maximum number of rows from dataset to use for testing (0 = 'all')",
                     0);

    parser.AddOption(args.numNodesToSkip, "skip", "", "Number of nodes in the beginning to skip", 0);

    parser.AddOption(args.fineTuneFullyConnectedNodes, "dense", "", "Fine-tune dense (fully-connected) layers", true);

    parser.AddOption(args.fineTuneConvolutionalNodes, "conv", "", "Fine-tune convolutional layers", true);

    parser.AddOption(args.dataFormat, "format", "", "Dataset format (GSDF, CIFAR, MNIST; default: guess)", "");

    parser.AddOption(args.l2Regularization, "l2regularization", "l2", "The L2 regularization parameter", 0.005);

    parser.AddOption(args.l1Regularization, "l1regularization", "l1", "The L1 regularization parameter", 0);

    parser.AddOption(args.desiredPrecision,
                     "desiredPrecision",
                     "",
                     "The desired duality gap at which to stop optimizing",
                     1.0e-4);

    parser.AddOption(args.maxEpochs, "maxEpochs", "e", "The maximum number of optimization epochs to run", 25);

    parser.AddOption(args.permute,
                     "permute",
                     "",
                     "Whether or not to randomly permute the training data before each epoch",
                     true);

    parser.AddOption(args.randomSeed, "randomSeed", "seed", "The random seed string", "ABCDEFG");

    parser.AddOption(args.reportFilename,
                     "reportFilename",
                     "",
                     "Output filename for report (empty for standard output)",
                     "");

    parser.AddOption(args.testOnly, "testOnly", "", "Report accuracy of model and exit", false);

    parser.AddOption(args.compile, "compile", "", "Compile the model when evaluating", true);

    parser.AddOption(args.verbose, "verbose", "v", "Turn on verbose mode", false);
}

void ParsedFineTuneArguments::AddArgs(utilities::CommandLineParser& parser)
{
    InitArguments(parser, *this);
}

ParsedFineTuneArguments ParseCommandLine(int argc, char* argv[])
{
    try
    {
        utilities::CommandLineParser commandLineParser(argc, argv);
        ParsedFineTuneArguments args;
        commandLineParser.AddOptionSet(args);
        commandLineParser.Parse();

        // if no input specified, print help and exit
        if (!args.mapLoadArguments.HasInputFilename())
        {
            if (commandLineParser.GetPositionalArgs().size() == 1)
            {
                args.mapLoadArguments.inputMapFilename = commandLineParser.GetPositionalArgs()[0];
            }
            else
            {
                throw utilities::CommandLineParserPrintHelpException(commandLineParser.GetHelpString());
            }
        }

        if (args.verbose)
        {
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        return args;
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        throw;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        throw;
    }
}
