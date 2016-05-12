
def generateDataset(numSamplesPerLabel, numDim):
    import numpy
    posCenter = numpy.random.uniform(-2, 2, numDim)
    negCenter = numpy.random.uniform(-2, 2, numDim)
    posData = numpy.random.normal(0.0, 1.0, (numSamplesPerLabel, numDim)) + posCenter
    posEx = numpy.hstack((numpy.ones((numSamplesPerLabel, 1)), posData))
    negData = numpy.random.normal(0.0, 1.0, (numSamplesPerLabel, numDim)) + negCenter
    negEx = numpy.hstack((-numpy.ones((numSamplesPerLabel, 1)), negData))
    dataset = numpy.vstack((posEx, negEx))
    numpy.random.shuffle(dataset)
    return dataset

def writeFile(dataset, filename):
    with  open(filename, 'w') as fp:
        for row in dataset:
            fp.write('{}'.format(row[0]))
            for i, v in enumerate(row[1:]):
                fp.write('\t{}:{}'.format(i+1, v))
            fp.write('\n')
            