// args.h

using utilities::ParsedArgSet;

/// A struct that holds the command line parameters for the trainer
///
struct SgdArguments
{
    int numEpochs = 0;
    double lossScale = 0;
    double l2Regularization = 0;
};

/// A subclass of cmd_arguments that knows how to Add its members to the command line parser
///
struct ParsedSgdArguments : SgdArguments, ParsedArgSet
{
    ParsedSgdArguments(CommandLineParser& parser)
    {
        AddArgs(parser);
    }

    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(numEpochs, "numEpochs", "ne", "Number of epochs over the data", 3);
        parser.AddOption(lossScale, "lossScale", "ls", "The regularization parameter lambda", 1.0);
        parser.AddOption(l2Regularization, "l2Regularization", "l2", "The regularization parameter that controls the amount of L2 regularization", 0.01);
    }
};
