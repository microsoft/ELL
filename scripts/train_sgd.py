""" Example recipe showing how to invoke some of the binaries to create, print, and compile a classifier."""
from recipeUtil import *

inputDatasetFile = args[0] # The file that contains the labeled dataset to use for training the classifier
outputModelFile = args[1]  # The file to write the resulting model to
outputHtmlFile = args[2]   # The file to write the model diagram to
outputCodeFile = args[3]   # The file to write the C output code to

# Train a classifier using sgd
runBinary('sgd', ['-idf', inputDatasetFile, '-osf', outputModelFile, '-dd', 'auto'])

# Print the classifier to an html file
runBinary('print', ['-isf', outputModelFile, '-osvg', outputHtmlFile])

# compile to C code
runBinary('compile', ['-isf', outputModelFile, '-ocf', outputCodeFile])
