""" Example recipe showing how to invoke some of the binaries to create, print, and compile a classifier."""
from recipeUtil import *

inputDatasetFilename = args[0] # The file that contains the labeled dataset to use for training the classifier
outputModelFilename = args[1]  # The file to write the resulting model to
outputHtmlFilename = args[2]   # The file to write the model diagram to
outputCodeFilename = args[3]   # The file to write the C output code to

# Train a classifier using sgd
runBinary('sgdTrainer', ['-idf', inputDatasetFilename, '-omf', outputModelFilename, '-dd', 'auto'])

# Print the classifier to an html file
runBinary('print', ['-imf', outputModelFilename, '-osvg', outputHtmlFilename])

# compile to C code
runBinary('compile', ['-imf', outputModelFilename, '-ocf', outputCodeFilename])
