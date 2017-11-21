from __future__ import print_function
import sys
import os
import numpy as np
import find_ell
import ELL
from testing import Testing

def get_accuracy(predictor, dataset, input_size):
    # compute training accuracy.
    total = dataset.NumExamples()
    true_positive = 0
    for i in range(total):
        example = dataset.GetExample(i)
        result = predictor.Predict(example.GetData())
        answer = np.argmax(result)
        expected = example.GetLabel()
        if answer == expected:
            true_positive += 1
    return true_positive / total

def test():
    testing = Testing()
    # -dd auto -sw 1 -sb 1 -sz 1 -pd 10 -l 2 -mp 5 -v --evaluationFrequency 1 -plf L2
    
    args = ELL.ProtoNNTrainerParameters()        
    args.projectedDimension = 10
    args.numPrototypesPerLabel = 5
    args.numLabels = 2
    args.sparsityW = 1
    args.sparsityB = 1
    args.sparsityZ = 1
    args.gamma = -1
    args.lossFunction = ELL.ProtoNNLossFunction.L2
    args.numIterations = 20
    args.numInnerIterations = 1
    args.numFeatures = 0
    args.verbose = True
    
    trainer = ELL.ProtoNNTrainer(args)
    
    dataset = ELL.AutoSupervisedDataset()
    testFile = os.path.join(find_ell.get_ell_root(), "examples/data/protonnTestData.txt")
    print("Loading: " + testFile)
    dataset.Load(testFile)
    
    total = dataset.NumExamples()
    features = dataset.NumFeatures()
    testing.ProcessTest("Proton dataset loaded", testing.IsEqual(int(total), 200))
    
    done = False
    trainer.SetDataset(dataset)

    print("Training...")
    while not done:
        trainer.Update()
        # todo: evaluate and see if we're done.
        done = True

    predictor = trainer.GetPredictor()
    
    accuracy = get_accuracy(predictor, dataset, features)
    print("Accuracy %f" % (accuracy))
    testing.ProcessTest("Proton accuracy test", testing.IsEqual(int(accuracy), 1))
    
    map = predictor.GetMap()
    map.Save("protonnTestData.ell")
    testing.ProcessTest("Saving  protonnTestData.ell", testing.IsEqual(os.path.isfile("protonnTestData.ell"), True))
    return 0

if __name__ == "__main__":
    test()