from __future__ import print_function
import ell
from testing import Testing

def makeStringVec(strings):
    v = ell.StringVector()
    for s in strings:
        v.push_back(s)
    return v

def testModelBuilder(testing):
    model = ell.ELL_Model()
    mb = ell.ELL_ModelBuilder()
    inArgs = makeStringVec(["3"])
    inputNode = mb.AddNode(model, "InputNode<double>", inArgs)
    outArgs = makeStringVec([inputNode.GetId()+".output"])
    mb.AddNode(model, "OutputNode<double>", outArgs)
    testing.ProcessTest("Testing ModelBuilder", testing.IsEqual(model.Size(), 2))

def test():
    testing = Testing()
    testModelBuilder(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0

if __name__ == '__main__':
    test()
