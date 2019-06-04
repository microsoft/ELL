import functools
import ell_helper
import ell
import os
import math
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


def test_multiply(testing):

    # Test a model that multiplies input vector by a constant matrix
    model = ell.model.Model()

    x = np.array([1, 2, 3])
    layout = ell.model.PortMemoryLayout([int(3)])
    input = model.AddInput(layout, ell.nodes.PortType.real)

    a = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9]).astype(np.float).reshape((3, 3))
    matrix = model.AddConstant(a.ravel(), ell.model.PortMemoryLayout([3, 3]), ell.nodes.PortType.real)
    multiply = model.AddMatrixMultiply(matrix, input)
    output = model.AddOutput(layout, multiply)

    map = ell.model.Map(model, input, output)

    expected = a.dot(x)
    result = map.Compute(x)

    testing.ProcessTest("Testing AddMatrixMultiply Compute",
                        testing.IsEqual(np.array(result), expected))

    compiled = map.Compile("host", "multiply_test", "predict")

    result2 = compiled.Compute(x)

    testing.ProcessTest("Testing AddMatrixMultiply Compiled",
                        testing.IsEqual(np.array(result2), expected))


class UnaryTest:
    def __init__(self, name, op, func):
        self.name = name
        self.op = op
        self.func = func


def hard_sigmoid(x):
    scale = 0.2
    bias = 0.5
    if x < -bias / scale:
        return 0
    if x > (1 - bias) / scale:
        return 1
    return (scale * x) + bias


def sigmoid(x):
    return 1.0 / (math.exp(-x) + 1)


def softmax(x):
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)


def sign(x):
    if x >= 0:
        return 1
    return -1


def test_unary(testing):

    # Test a model that performs a unary operation
    for test in [UnaryTest("abs", ell.nodes.UnaryOperationType.abs, lambda x: abs(x)),
                 UnaryTest("cos", ell.nodes.UnaryOperationType.cos, lambda x: math.cos(x)),
                 UnaryTest("exp", ell.nodes.UnaryOperationType.exp, lambda x: math.exp(x)),
                 UnaryTest("hardSigmoid", ell.nodes.UnaryOperationType.hardSigmoid, lambda x: hard_sigmoid(x)),
                 UnaryTest("log", ell.nodes.UnaryOperationType.log, lambda x: math.log(x)),
                 UnaryTest("sigmoid", ell.nodes.UnaryOperationType.sigmoid, lambda x: sigmoid(x)),
                 UnaryTest("sign", ell.nodes.UnaryOperationType.sign, lambda x: sign(x)),
                 UnaryTest("sin", ell.nodes.UnaryOperationType.sin, lambda x: math.sin(x)),
                 UnaryTest("softmax", ell.nodes.UnaryOperationType.softmax, lambda x: softmax(x)),
                 UnaryTest("sqrt", ell.nodes.UnaryOperationType.sqrt, lambda x: math.sqrt(x)),
                 UnaryTest("square", ell.nodes.UnaryOperationType.square, lambda x: x * x),
                 UnaryTest("tanh", ell.nodes.UnaryOperationType.tanh, lambda x: math.tanh(x))]:

        model = ell.model.Model()

        layout = ell.model.PortMemoryLayout([int(10)])
        input = model.AddInput(layout, ell.nodes.PortType.smallReal)

        a = np.array(range(10)).astype(np.float32) + 1
        multiply = model.AddUnaryOperation(input, test.op)
        output = model.AddOutput(layout, multiply)

        expected = np.array([test.func(x) for x in a])
        if test.name == "softmax":
            expected = softmax(a)

        map = ell.model.Map(model, input, output)

        result = map.Compute(a)

        testing.ProcessTest("Testing AddUnaryOperation {}".format(test.name),
                            testing.IsEqual(np.array(result), expected, tol=1e-6))

        compiled = map.Compile("host", "test", "predict")

        result = compiled.Compute(a)

        testing.ProcessTest("Testing AddUnaryOperation Compiled {}".format(test.name),
                            testing.IsEqual(np.array(result), expected, tol=1e-6))


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


class TypeCastInfo:
    def __init__(self, name, t):
        self.name = name
        self.t = t

    def create_vector(self, len):
        if self.t == ell.nodes.PortType.real:
            return ell.math.DoubleVector(len)
        elif self.t == ell.nodes.PortType.smallReal:
            return ell.math.FloatVector(len)
        elif self.t == ell.nodes.PortType.integer:
            return ell.math.IntVector(len)
        elif self.t == ell.nodes.PortType.bigInt:
            return ell.math.Int64Vector(len)
        raise Exception("Unsupported type")

    def cast_vector(self, a):
        if self.t == ell.nodes.PortType.real:
            return a.astype(np.float)
        elif self.t == ell.nodes.PortType.smallReal:
            return a.astype(np.float32)
        elif self.t == ell.nodes.PortType.integer:
            return a.astype(np.int)
        elif self.t == ell.nodes.PortType.bigInt:
            return a.astype(np.int64)
        raise Exception("Unsupported type")


def test_typecast(testing):
    # Test a model that has differen types callbacks.
    for t in [TypeCastInfo("real", ell.nodes.PortType.real),
              TypeCastInfo("smallReal", ell.nodes.PortType.smallReal),
              TypeCastInfo("integer", ell.nodes.PortType.integer),
              # TypeCastInfo("bigInt", ell.nodes.PortType.bigInt)
              ]:

        model = ell.model.Model()

        # input to a SourceNode is a timestamp.
        layout = ell.model.PortMemoryLayout([int(10)])
        input = model.AddInput(layout, ell.nodes.PortType.real)
        cast = model.AddTypeCast(input, t.t)

        output = model.AddOutput(layout, cast)
        map = ell.model.Map(model, input, output)

        a = np.array(range(10)).astype(np.float) / 2
        expected = np.array(a)

        i = ell.math.DoubleVector(a)
        o = t.create_vector(len(a))
        expected = t.cast_vector(a)
        map.ComputeMultiple([i], [o])

        testing.ProcessTest("Testing TypeCast to type {} Compute".format(t.name),
                            testing.IsEqual(np.array(o), expected))

        compiled = map.Compile("host", "test", "predict")
        o = t.create_vector(len(a))
        compiled.ComputeMultiple([i], [o])
        testing.ProcessTest("Testing TypeCast to type {} Compiled".format(t.name),
                            testing.IsEqual(np.array(o), expected))


def test():
    testing = Testing()
    test_callbacks(testing)
    test_typecast(testing)
    test_unary(testing)
    test_multiple(testing)
    #test_multiply(testing)  # bugbug: crashing on Linux...
    test_bitcode(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0


if __name__ == '__main__':
    test()
