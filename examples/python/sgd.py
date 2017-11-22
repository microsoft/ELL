import sys

# Different configurations put the DLL in different places. 
sys.path.append('.')
sys.path.append('../../interfaces/python')
sys.path.append('../../interfaces/python/Release')
sys.path.append('../../interfaces/python/Debug')

import ell
from ell import * # pull everything into main namespace

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
    outModelFilename = getArg(2, 'modelOut.xml')
    randomSeed = 'abcdefg'
    numEpochs = 3
    epochSize = 0 # 0 means "all"
    l2Regularization = 0.01
    dataDimension = 21
    
    #  Get the dataset
    dataset = GetDataset(dataFilename);

    # create sgd trainer    
    loss = LogLoss()
    params = SGDTrainerParameters()
    params.regularization = l2Regularization
    trainer = LogLossSGDTrainer(dataDimension, loss, params)

    # evaluator = LinearLogLossClassificationEvaluator(loss)

    numExamples = dataset.NumExamples()
    if not epochSize or epochSize >= numExamples:
        epochSize = numExamples

    print "Running SGD over dataset of size {0} x {1}".format(numExamples, dataDimension)
    rng = GetRandomEngine(randomSeed)

    for epoch in xrange(numEpochs):
        # randomly permute the data
        dataset.RandomPermute(rng, epochSize)

        # iterate over the entire permuted dataset
        trainSetIterator = dataset.GetIterator(0, epochSize)
        trainer.Update(trainSetIterator)

        evalIterator = dataset.GetIterator()
        predictor = trainer.GetPredictor()
        #evaluator.Evaluate(evalIterator, predictor)
        #print "{}".format(evaluator.GetLastLoss())
        
    #print "binary classification evaluation"
    #print "Loss: {0}\tError: {1}".format(evaluator.GetLastLoss(), evaluator.GetLastError())
    
    # update the map with the newly learned layers
    predictor = trainer.GetPredictor()
    
    # output the model
    # if outModelFilename:
    #     model = Model()
    #     inputNode = model.AddNode<model::InputNode<double>>(predictor.GetDimension())
    #     model.AddNode<nodes::LinearPredictorNode>(inputNode.output, predictor)
    #     SaveModel(model, outModelFilename)
    
sgd()

