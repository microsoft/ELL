import os
import numpy as np
from testing import Testing
import find_ell
import ell

def exampleTest(example):
    # Now test the IDataVector of the example
    av = example.GetData()
    l = example.GetLabel()

    # test we can copy AutoDataVector to DoubleVector
    v = ell.math.DoubleVector()
    av.CopyTo(v)

    x = np.asarray(av.ToArray())
    y = np.asarray(v)
    np.testing.assert_equal(x, y)

def test():
    testing = Testing()
    dataset = ell.data.AutoSupervisedDataset()
    dataset.Load(os.path.join(find_ell.get_ell_root(), "examples/data/testData.txt"))
    num = dataset.NumExamples()
    print("Number of Examples:", num)
    testing.ProcessTest("Dataset NumExamples test", testing.IsEqual(int(num), 200))

    features = dataset.NumFeatures()
    print("Number of Features:", features)
    testing.ProcessTest("Dataset NumFeatures test", testing.IsEqual(int(features), 21))

    for i in range(num):
        exampleTest(dataset.GetExample(i))

    testing.ProcessTest("Dataset eumeration test", True)

    return 0

if __name__ == "__main__":
    test()
