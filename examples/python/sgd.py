import sys

# Different environments put the DLL in different places. 
# TODO: Fix this to somehow only add the correct path.
sys.path.append('.')
sys.path.append('../../interfaces/python')
sys.path.append('../../interfaces/python/Release')
sys.path.append('../../interfaces/python/Debug')

import EMLL
from EMLL import * # pull everything into main namespace

def getArg(n, missingValue=''):
    if n < len(sys.argv):
        return sys.argv[n]
    return missingValue

dataset = None
trainer = None
evaluator = None
def sgd():
    global trainer
    global dataset
    global evaluator

    # Parameters
    dataFilename = getArg(1)
    inMapFilename = getArg(2)
    outMapFilename = getArg(3, 'mapOut.xml')
    coordinateString = getArg(4, 'e')
    randomSeed = 'abcdefg'
    numEpochs = 3
    epochSize = 0 # 0 means "all"
    l2Regularization = 0.01
    dataDimension = 3

    # Read model from file
    model = LoadModel(inMapFilename);
    
    # get output coordinate list and create the map
    outputCoordinates = BuildCoordinateList(model, dataDimension, coordinateString);
    map = Map(model, outputCoordinates);

    #  Get the dataset
    dataset = GetDataset(dataFilename, map);

    # create sgd trainer    
    loss = LogLoss()
    params = SGDIncrementalTrainerParameters()
    params.regularization = l2Regularization
    trainer = LogLossSGDTrainer(outputCoordinates.Size(), loss, params)

    # create evaluator
    evaluator = LinearLogLossClassificationEvaluator(loss)

    numExamples = dataset.NumExamples()
    if not epochSize or epochSize >= numExamples:
        epochSize = numExamples

    print "Running SGD over dataset of size {0} x {1}".format(numExamples, outputCoordinates.Size())
    print "Output coordinates: {0}".format(outputCoordinates)
    rng = GetRandomEngine(randomSeed)

    for epoch in xrange(numEpochs):
        # randomly permute the data
        dataset.RandomPermute(rng, epochSize)

        # iterate over the entire permuted dataset
        trainSetIterator = dataset.GetIterator(0, epochSize) # mem leak comes from here
        trainer.Update(trainSetIterator)  

        evalIterator = dataset.GetIterator()
        predictor = trainer.GetPredictor()
        evaluator.Eval(evalIterator, predictor) 
        
    print "Training Error:"
    print "binary classification evaluation"
    print "loss\terror"
    print "{0}\t{1}".format(evaluator.GetLastLoss(), evaluator.GetLastError())
    
    # update the map with the newly learned layers
#    predictor = trainer.GetPredictor()
#    predictor.AddToModel(model, map.GetOutputCoordinateList())
    
    # output the map
#    model.Save(outMapFilename)
    
sgd()

