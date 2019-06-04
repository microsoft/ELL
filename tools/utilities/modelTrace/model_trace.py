#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     model_compare.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################
import argparse
import math
import os
import sys
import random

script_path = os.path.dirname(__file__)
sys.path += [os.path.join(script_path, "..", "pythonlibs")]
import find_ell  # noqa 401
import ell
import numpy as np


class DoubleCallbackWrapper(ell.math.DoubleCallbackBase):
    def __init__(self, name, owner):
        super(DoubleCallbackWrapper, self).__init__()
        self.name = name
        self.owner = owner

    def Run(self, data):
        self.owner.handle_callback(self.name, data)
        return False


class FloatCallbackWrapper(ell.math.FloatCallbackBase):
    def __init__(self, name, owner):
        super(FloatCallbackWrapper, self).__init__()
        self.name = name
        self.owner = owner

    def Run(self, data):
        self.owner.handle_callback(self.name, data)
        return False


class ModelTracer():
    def __init__(self, vector_filler):
        self.sinkMap = {}
        self.sinkIndex = 0
        self.map = None
        self.vector_filler = vector_filler
        self.wrappers = []
        self.recording = {}
        self.compiled = False

    def create_test_vector(self, size):
        result = []
        for i in range(size):
            result += [self.vector_filler(i)]
        return result

    def has_neural_predictor(self, model):
        iter = model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            name = node.GetRuntimeTypeName()
            if "NeuralNetworkPredictorNode" in name:
                return True
            iter.Next()
        return False

    def print_nodes(self, model):
        iter = model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            name = node.GetRuntimeTypeName()
            print(node.GetId(), name)
            iter.Next()

    def get_sink_name(self, node):
        name = node.GetRuntimeTypeName()
        if '<' in name:
            name = name[0:name.index('<')]
        self.sinkIndex += 1
        name = "Sink_{}_{}".format(name, self.sinkIndex)
        self.sinkMap[name] = node
        return name

    def add_debug_sink_node(self, map, node):
        try:
            model = map.GetModel()
            outputPort = node.GetOutputPort("output")
            portMemoryLayout = outputPort.GetMemoryLayout()
            name = self.get_sink_name(node)
            model.AddSink(node, portMemoryLayout, name)
            portType = outputPort.GetOutputType()
            if portType == ell.nodes.PortType.smallReal:
                wrapper = FloatCallbackWrapper(name, self)
                map.RegisterSinkCallbackFloat(wrapper, name)
                # This is a bit tricky.  We are passing a pointer to a python object to ELL to use
                # as a callback, it has to be a pointer for the callback to work.  So we also here
                # have to keep this wrapper object alive so ELL doesn't end up calling dead memory.
                self.wrappers += [wrapper]
            elif portType == ell.nodes.PortType.real:
                wrapper = DoubleCallbackWrapper(name, self)
                map.RegisterSinkCallbackDouble(wrapper, name)
                self.wrappers += [wrapper]
            else:
                raise Exception("Port type not supported: " + str(portType))
        except Exception as e:
            print("Error adding SinkNode : {}".format(e))

    def add_debug_sink_nodes(self, map):
        model = map.GetModel()
        iter = model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            name = node.GetRuntimeTypeName()
            if "InputNode" in name:
                pass
            elif "OutputNode" in name:
                pass
            elif "ConstantNode" in name:
                pass
            else:
                self.add_debug_sink_node(map, node)
            iter.Next()
        return

    def create_vector(self, size, portType, zero=False):
        if zero:
            vector = [0] * size
        else:
            vector = self.create_test_vector(size)
        if portType == ell.nodes.PortType.smallReal:
            return ell.math.FloatVector(vector)
        elif portType == ell.nodes.PortType.real:
            return ell.math.DoubleVector(vector)
        elif portType == ell.nodes.PortType.integer:
            return ell.math.IntVector(vector)
        else:
            raise Exception("Port type not supported: " + str(portType))

    def create_input_list(self, map):
        result = []
        for i in range(map.NumInputs()):
            ml = map.GetInputLayout(i)
            size = np.product(list(ml.size))
            result += [self.create_vector(size, map.GetInputType(i))]
        return result

    def create_output_list(self, map):
        result = []
        for i in range(map.NumOutputs()):
            ml = map.GetOutputLayout(i)
            size = np.product(list(ml.size))
            result += [self.create_vector(size, map.GetInputType(i), zero=True)]
        return result

    def handle_callback(self, name, data):
        data = np.array(data).astype(np.float32)
        if not self.compiled:
            self.recording[name] = (data, None)
        else:
            previous = None
            if name in self.recording:
                previous, _ = self.recording[name]
            self.recording[name] = (previous, data)

    def compare(self, model_file):
        map = ell.model.Map(model_file)
        model = map.GetModel()
        self.print_nodes(model)
        print("")
        if self.has_neural_predictor(model):
            print("Found a NeuralNetworkPredictorNode, refining once to expand it")
            map.Refine(1)
            model = map.GetModel()
            print("")
            self.print_nodes(model)

        self.add_debug_sink_nodes(map)

        model = map.GetModel()
        inputs = self.create_input_list(map)
        outputs = self.create_output_list(map)

        map.ComputeList(inputs + outputs)
        i = 0
        for o in outputs:
            data = np.array(o).astype(np.float32)
            name = "Results {}".format(i)
            self.recording[name] = (data, None)
            i += 1

        map.Reset()
        self.compiled = True
        compiledMap = map.Compile("host", "test", "predict")
        compiledMap.ComputeList(inputs + outputs)

        i = 0
        for o in outputs:
            data = np.array(o).astype(np.float32)
            name = "Results {}".format(i)
            previous = None
            if name in self.recording:
                previous, _ = self.recording[name]
            self.recording[name] = (previous, data)
            i += 1

    def truncate(self, data, decimals=3):
        scale = pow(10, decimals)
        return np.array([math.floor(x * scale) / scale for x in data])

    def write_row(self, file, label, data):
        file.write("{},{}\n".format(label, ",".join(list([str(x) for x in data]))))

    def report(self, output_csv, precision):
        with open(output_csv, "w") as f:
            for name in self.recording:
                a, b = self.recording[name]
                if a is None:
                    f.write("Compute_{},callback didn't happen\n".format(name))
                    self.write_row(f, "Compile_{}".format(name), b)
                    print("Compute_{},callback didn't happen".format(name))
                elif b is None:
                    self.write_row(f, "Compute_{}".format(name), a)
                    f.write("Compile_{},callback didn't happen\n".format(name))
                    print("Compile_{},callback didn't happen".format(name))
                else:
                    self.write_row(f, "Compute_{}".format(name), a)
                    self.write_row(f, "Compile_{}".format(name), b)

                    a = self.truncate(a, precision)
                    b = self.truncate(b, precision)

                    if len(a) != len(b):
                        print("{} buffer sizes differ, compute={}, compiled={}".format(name, len(a), len(b)))
                    elif all(a == b):
                        print("{} output matches!".format(name))
                    else:
                        print("Compute_{}: min={}, max={}, mean={}".format(
                            name, np.min(a), np.max(a), np.mean(a)))
                        print("Compile_{}: min={}, max={}, mean={}".format(
                            name, np.min(b), np.max(b), np.mean(b)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Trace the output of all the nodes in a given model")
    parser.add_argument("model", help="Path to ELL model file")
    parser.add_argument("--input_generator", help="Values to pass as inputs (zero, one, random)",
                        choices=["zero", "one", "random"],
                        default="one")
    parser.add_argument("--output", help="Name of output .csv file to save results (default output.csv)",
                        default="output.csv")
    parser.add_argument("--precision", help="Number of decimal places to compare (default 3)", type=int,
                        default=3)
    args = parser.parse_args()

    if args.input_generator == "zero":
        c = ModelTracer(lambda i: 0)
    elif args.input_generator == "one":
        c = ModelTracer(lambda i: 1)
    else:
        c = ModelTracer(lambda i: random.uniform(0, 1))
    c.compare(args.model)
    c.report(args.output, args.precision)
