###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     train_protonn.py
#  Authors:  Chris Lovett
#  Requires: Python 3.x
#
###############################################################################
import sys
import numpy as np
import os
import find_ell
import ell

def main():
    """ Train a ProtoNN model using the given MNIST dataset """

    model_file = "Train-28x28_cntk_text.ds"
    if not os.path.isfile(model_file):
        print("Could not find '%s'" % (model_file))
        print("See convert_dataset.py")
        sys.exit(1)

    print("Loading '%s'" % (model_file))
    dataset = ell.AutoSupervisedDataset()
    test_file = model_file
    dataset.Load(test_file)

    args = ell.ProtoNNTrainerParameters()

    # This Python code is doing the equivalent of the following command line:
    # protoNNTrainer -v --inputDataFilename Train-28x28_sparse.txt -dd 784 -sw 1 
    # -sb 0.3 -sz 0.8 -pd 10 -l 10 -mp 20 -outputModelFilename mnist.model 
    # --evaluationFrequency 1 -plf L4

    args.projectedDimension = 10
    args.numPrototypesPerLabel = 20
    args.numLabels = 10
    args.sparsityW = 1
    args.sparsityB = 0.3
    args.sparsityZ = 0.8
    args.gamma = -1
    args.lossFunction = ell.ProtoNNLossFunction.L4
    args.numIterations = 20
    args.numInnerIterations = 1
    args.numFeatures = 784
    args.verbose = True

    trainer = ell.ProtoNNTrainer(args)

    trainer.SetDataset(dataset)

    print("Training...")
    trainer.Update()

    predictor = trainer.GetPredictor()

    print("Computing training accuracy...")
    total = dataset.NumExamples()
    true_positive = 0
    for i in range(total):
        example = dataset.GetExample(i)
        result = predictor.Predict(example.GetData())
        answer = np.argmax(result)
        expected = example.GetLabel()
        if answer == expected:
            true_positive += 1

    accuracy = true_positive / total
    print("Accuracy %f" % (accuracy))

    print("Saving model 'mnist.ell'")
    map = predictor.GetMap()
    map.Save("mnist.ell")

if __name__ == "__main__":
    main()
