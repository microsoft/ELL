# finetune utility

This tool reoptimizes the weights in fully-connected and convolutional layers of a model. 

## Usage

```shell
$ bin/finetune --help
Usage: finetune [options]

        --inputMapFilename (-imap) []     Path to the input map file
        --inputModelFilename (-imf) []    Path to the input model file
        --modelInputs (-in) []            Model inputs to use
        --modelOutputs (-out) []          Model outputs to use
        --defaultInputSize (-d) [1]       Default size of input node
        --outputMapFilename (-omf) []     Path to the output map file (empty for standard out, 'null' for no output)
        --trainDataFilename []            Path to the input data file
        --trainDataDirectory []           Directory for the input data file
        --trainDataDimension []           Number of elements to read from each data vector
        --testDataFilename []             Path to the input data file
        --testDataDirectory []            Directory for the input data file
        --testDataDimension []            Number of elements to read from each data vector
        --targetPortElements (-tpe) []    The port elements of the pre-trained model to use as input to the subsequent linear predictor e.g. "1115.output" to usethe full output from Node 1115
        --print [false]                   Print the model
        --multiClass (-mc) [true]         Indicates whether the input dataset is multi-class or binary.
        --maxTrainingRows [0]             Maximum number of rows from dataset to use for training (0 = 'all')
        --maxTestingRows [0]              Maximum number of rows from dataset to use for testing (0 = 'all')
        --skip [0]                        Number of nodes in the beginning to skip
        --dense [true]                    Fine-tune dense (fully-connected) layers
        --conv [true]                     Fine-tune convolutional layers
        --format []                       Dataset format (GSDF, CIFAR, MNIST; default: guess)
        --l2regularization (-l2) [0.005]  The L2 regularization parameter
        --l1regularization (-l1) [0]      The L1 regularization parameter
        --desiredPrecision [0.0001]       The desired duality gap at which to stop optimizing
        --maxEpochs (-e) [25]             The maximum number of optimization epochs to run
        --permute [true]                  Whether or not to randomly permute the training data before each epoch
        --randomSeed (-seed) [ABCDEFG]    The random seed string
        --reportFilename []               Output filename for report (empty for standard output)
        --testOnly [false]                Report accuracy of model and exit
        --compile [true]                  Compile the model when evaluating
        --verbose (-v) [false]            Turn on verbose mode

General options
        --help (-h) [false]               Print help and exit
```
