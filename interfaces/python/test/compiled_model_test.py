import ell
import os
import time
import numpy as np
from testing import Testing


def test_bitcode(testing):
    # Load the map created by proton trainer test and compile it
    map = ell.model.Map("protonnTestData.ell")
    compiledMap = map.Compile("host", "protonn", "predict")
    compiledMap.WriteBitcode("protonnTestData.bc")

    testing.ProcessTest("Test bitcode generated for protonnTestData.ell",
                        os.path.isfile("protonnTestData.bc"))


def test_multiple(testing):
    # Test a model that has multiple inputs and outputs
    model = ell.model.Model()

    layout = ell.model.PortMemoryLayout([int(10)])
    input1Node = model.AddInput(layout, ell.nodes.PortType.real)
    input2Node = model.AddInput(layout, ell.nodes.PortType.real)
    addNode = model.AddBinaryOperation(input1Node, input2Node, ell.nodes.BinaryOperationType.add)
    diffNode = model.AddBinaryOperation(input1Node, input2Node, ell.nodes.BinaryOperationType.subtract)

    output1Node = model.AddOutput(layout, addNode)
    output2Node = model.AddOutput(layout, diffNode)

    map = ell.model.Map(model, ell.nodes.InputNodeList([input1Node, input2Node]),
                        ell.nodes.OutputNodeList([output1Node, output2Node]))

    x = np.array(range(10))
    y = np.array(range(10)) * 2
    i1 = ell.math.DoubleVector(x)
    i2 = ell.math.DoubleVector(y)
    o1 = ell.math.DoubleVector(10)
    o2 = ell.math.DoubleVector(10)
    map.ComputeMultiple([i1, i2], [o1, o2])
    testing.ProcessTest("Testing multiple inputs and outputs from Map::Compute",
                        testing.IsEqual(np.array(o1), x + y) and testing.IsEqual(np.array(o2), x - y))

    compiled = map.Compile("host", "test", "predict")
    compiled.ComputeMultiple([i1, i2], [o1, o2])
    testing.ProcessTest("Testing multiple inputs and outputs from CompiledMap::Compute",
                        testing.IsEqual(np.array(o1), x + y) and testing.IsEqual(np.array(o2), x - y))


def fill_with_ones_double(data):
    ones = ell.math.DoubleVector([1] * data.size())
    data.copy_from(ones)
    return True


def fill_with_ones_float(data):
    ones = ell.math.FloatVector([1] * data.size())
    data.copy_from(ones)
    return True


def fill_with_ones_int(data):
    ones = ell.math.IntVector([1] * data.size())
    data.copy_from(ones)
    return True


def fill_with_ones_int64(data):
    ones = ell.math.Int64Vector([1] * data.size())
    data.copy_from(ones)
    return True


class CallbackTypeInfo:
    def __init__(self, name, type):
        self.name = name
        self.type = type

    def get_function(self):
        if self.type == ell.nodes.PortType.real:
            return fill_with_ones_double
        elif self.type == ell.nodes.PortType.smallReal:
            return fill_with_ones_float
        elif self.type == ell.nodes.PortType.integer:
            return fill_with_ones_int
        elif self.type == ell.nodes.PortType.bigInt:
            return fill_with_ones_int64
        return True


class TestSink:
    def handle_callback(self, data):
        self.data = ell.math.DoubleVector(data)


def test_callbacks(testing):
    for t in [CallbackTypeInfo("real", ell.nodes.PortType.real),
              CallbackTypeInfo("smallReal", ell.nodes.PortType.smallReal),
              CallbackTypeInfo("integer", ell.nodes.PortType.integer),
              # CallbackTypeInfo("bigInt", ell.nodes.PortType.bigInt)
              ]:

        # Test a model that has SourceNode and SinkNode callbacks.
        model = ell.model.Model()

        dataLayout = ell.model.PortMemoryLayout([10])
        layout = ell.model.PortMemoryLayout([int(1)])
        # input to a SourceNode is a timestamp.
        timeInput = model.AddInput(layout, ell.nodes.PortType.real)
        source = model.AddSource(timeInput, ell.nodes.PortType.real, ell.model.PortMemoryLayout([int(10)]), "Source1")
        source.RegisterCallback(t.get_function())

        a = np.array(range(10)).astype(np.float)
        constant1 = model.AddConstant(a.ravel(), dataLayout, ell.nodes.PortType.real)

        addNode = model.AddBinaryOperation(source, constant1, ell.nodes.BinaryOperationType.add)

        sink = model.AddSink(addNode, dataLayout, "Sink1")
        sinkCallback = TestSink()
        sink.RegisterCallback(sinkCallback.handle_callback)

        output = model.AddOutput(dataLayout, sink)

        map = ell.model.Map(model, timeInput, output)

        ones = np.array([1] * len(a))
        expected = a + ones
        result = map.Compute([time.time()])

        sinkOutput = sinkCallback.data

        testing.ProcessTest("Testing SourceNode {} callbacks from Map::Compute".format(t.name),
                            testing.IsEqual(np.array(result), expected))
        testing.ProcessTest("Testing SinkNode {} callbacks from Map::Compute".format(t.name),
                            testing.IsEqual(np.array(sinkOutput), expected))

        # test callbacks from compiled map too
        sinkCallback.data = []
        compiled = map.Compile("host", "test", "predict")
        result = compiled.Compute([time.time()])
        sinkOutput = sinkCallback.data
        testing.ProcessTest("Testing SourceNode {} callbacks from CompiledMap::Compute".format(t.name),
                            testing.IsEqual(np.array(result), expected))
        testing.ProcessTest("Testing SinkNode {} callbacks from CompiledMap::Compute".format(t.name),
                            testing.IsEqual(np.array(sinkOutput), expected))



def test():
    # this test only tests the model interface.  If you are testing node types then use nodes_test.py
    testing = Testing()
    test_callbacks(testing)
    test_multiple(testing)
    test_bitcode(testing)

    if testing.DidTestFail():
        return 1
    else:
        return 0
    if x > (1 - bias) / scale:
        return 1
    return (scale * x) + bias


if __name__ == '__main__':
    test()
