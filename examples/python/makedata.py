import numpy

def generateDataset():
    numDim = 20
    numSamples = 100 # (per label)
    posCenter = numpy.random.uniform(-2, 2, numDim)
    negCenter = numpy.random.uniform(-2, 2, numDim)
    posEx = numpy.random.normal(0.0, 1.0, (numSamples, numDim)) + posCenter
    negEx = numpy.random.normal(0.0, 1.0, (numSamples, numDim)) + negCenter
    # TODO: add pos and neg examples to a big list, randomize order, and output to a file    