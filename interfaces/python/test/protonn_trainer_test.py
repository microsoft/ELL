import sys
import os
import numpy as np
import find_ell
import ell
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

    args = ell.trainers.ProtoNNTrainerParameters()
    args.projectedDimension = 10
    args.numPrototypesPerLabel = 5
    args.numLabels = 2
    args.sparsityW = 1
    args.sparsityB = 1
    args.sparsityZ = 1
    args.gamma = -1
    args.lossFunction = ell.trainers.ProtoNNLossFunction.L2
    args.numInnerIterations = 1
    args.numFeatures = 0
    args.verbose = True

    trainer = ell.trainers.ProtoNNTrainer(args)

    dataset = ell.data.AutoSupervisedDataset()
    testFile = os.path.join(find_ell.get_ell_root(), "examples/data/protonnTestData.txt")
    print("Loading: " + testFile)
    dataset.Load(testFile)

    total = dataset.NumExamples()
    features = dataset.NumFeatures()
    testing.ProcessTest("Proton dataset loaded", testing.IsEqual(int(total), 200))

    trainer.SetDataset(dataset)

    numIterations = 20

    print("Training...")
    for i in range(numIterations):
        trainer.Update()

    predictor = trainer.GetPredictor()

    accuracy = get_accuracy(predictor, dataset, features)
    print("Accuracy %f" % (accuracy))
    testing.ProcessTest("Proton accuracy test", testing.IsEqual(int(accuracy), 1))

    map = predictor.GetMap()
    map.Save("protonnTestData.ell")
    testing.ProcessTest("Saving  protonnTestData.ell", testing.IsEqual(os.path.isfile("protonnTestData.ell"), True))

    if testing.DidTestFail():
        raise Exception("protonn_trainer_test failed")

    return 0

if __name__ == "__main__":
    test()
