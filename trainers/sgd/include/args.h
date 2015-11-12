// args.h

using utilities::ParsedArgSet;

/// A struct that holds the command line parameters for the trainer
///
struct sgd_arguments
{
    int num_epochs = 0;
    double loss_scale = 0;
    double l2_regularization = 0;
};

/// A subclass of cmd_arguments that knows how to Add its members to the command line parser
///
struct ParsedSgdArguments : sgd_arguments, ParsedArgSet
{
    ParsedSgdArguments(CommandLineParser& parser)
    {
        AddArgs(parser);
    }

    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(num_epochs, "num_epochs", "ne", "Number of epochs over the data", 3);
        parser.AddOption(loss_scale, "loss_scale", "ls", "The regularization parameter lambda", 1.0);
        parser.AddOption(l2_regularization, "l2_regularization", "l2", "The regularization parameter that controls the amount of L2 regularization", 0.01);
    }
};
