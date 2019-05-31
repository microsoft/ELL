#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     wrap_test.py
#  Authors:  Chris Lovett, Kern Handa
#
#  Requires: Python 3.x
#
####################################################################################################
import gc
import os
import sys
from shutil import copyfile, rmtree
import time

import numpy as np

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(script_path, '..', '..', '..', 'tools', 'utilities', 'pythonlibs')]
import find_ell
import ell

ell_build_dir = find_ell.find_ell_build()
sys.path += [os.path.join(ell_build_dir, "tools", "wrap")]
import wrap
import buildtools


def wrap_model(model, target_dir, language):
    builder = wrap.ModuleBuilder()
    args = ["--model_file", model,
            "--outdir", os.path.join(target_dir, "model"),
            "--language", language,
            "--target", "host",
            "--module_name", "model"]
    builder.parse_command_line(args)
    builder.run()


def make_project(target_dir):

    build_dir = os.path.join(target_dir, "build")
    if os.path.isdir(build_dir):
        rmtree(build_dir)
    os.makedirs(build_dir)

    current_path = os.getcwd()
    os.chdir(build_dir)
    cmd = buildtools.EllBuildTools(find_ell.get_ell_root())
    cmake = ["cmake", ".."]
    if os.name == 'nt':
        cmake = ["cmake", "-G", "Visual Studio 15 2017 Win64", "-Thost=x64", ".."]
    cmd.run(cmake, print_output=True)

    make = ["make"]
    if os.name == 'nt':
        make = ["cmake", "--build", ".", "--config", "Release"]
    cmd.run(make, print_output=True)
    os.chdir(current_path)


def create_model(callbacks=True):

    model = ell.model.Model()
    mb = ell.model.ModelBuilder()

    # we want an input vectors of size 10
    shape = ell.math.TensorShape(1, 1, 10)
    layout = ell.model.PortMemoryLayout(shape)

    if callbacks:
        # add node representing input (input nodes have no input, they are the input)
        # the OutputPort of the InputNode is the input data they pass along to the next node
        inputNode = mb.AddInputNode(model, ell.model.PortMemoryLayout([1]), ell.nodes.PortType.real)
        inputLink = inputNode.GetOutputPort("output")

        # clock node is required to setup the timing of the callbacks.
        clockNode = mb.AddClockNode(model, ell.nodes.PortElements(inputLink), float(30), float(60),
                                    "LagNotification")
        clockLink = clockNode.GetOutputPort("output")

        # add a SourceNode that gets input from the application
        sourceNode = mb.AddSourceNode(
            model, ell.nodes.PortElements(clockLink),
            ell.nodes.PortType.real, layout, "SourceCallback")
        sourceLink = sourceNode.GetOutputPort("output")
    else:
        inputNode = mb.AddInputNode(model, layout, ell.nodes.PortType.real)
        sourceNode = inputNode
        sourceLink = sourceNode.GetOutputPort("output")

    # add a constant vector to the input provided in the InputCallback
    constNode = mb.AddConstantNode(model, [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0],
                                   layout, ell.nodes.PortType.real)
    constLink = constNode.GetOutputPort("output")

    print("sourceLink size: {}".format(sourceLink.GetMemoryLayout().size.size()))
    print("constLink size: {}".format(constLink.GetMemoryLayout().size.size()))

    addNode = mb.AddBinaryOperationNode(model, ell.nodes.PortElements(sourceLink),
                                        ell.nodes.PortElements(constLink), ell.nodes.BinaryOperationType.add)
    addLink = addNode.GetOutputPort("output")

    if callbacks:
        # add a SinkNode to send this output to the application via OutputCallback
        # (setup a condition for the sink node that is always true).
        sinkNode = mb.AddSinkNode(model, ell.nodes.PortElements(addLink), layout, "SinkCallback")
        sinkLink = sinkNode.GetOutputPort("output")
    else:
        sinkLink = addLink

    # add a node representing output from the model.
    outputNode = mb.AddOutputNode(model, layout, ell.nodes.PortElements(sinkLink))
    outputResult = outputNode.GetOutputPort("output")

    # create Map that wraps the model.
    map = ell.model.Map(model, inputNode, ell.nodes.PortElements(outputResult))
    return map


def memory_test(func):
    # make sure we don't leak.
    func()
    gc.collect()
    before = dict((id(o), o) for o in gc.get_objects())
    gc.collect()
    func()
    gc.collect()
    after = dict((id(o), o) for o in gc.get_objects())
    if id(before) in after:
        after.pop(id(before))
    for key in after:
        if key in before:
            before.pop(key)

    # skip python built in types, we only care about leaks in ELL types.
    leaks = {}
    for k in before:
        o = before[k]
        if not isinstance(o, list) and not isinstance(o, dict) and not isinstance(o, tuple) and not isinstance(o, set):
            leaks[k] = o

    if len(leaks) != 0:
        print("### FAILED wrap_test python detected a memory leak in our predict call")
        for k in leaks:
            print("### Leak {}: {}\n".format(k, str(type(leaks[k]))))
        return 1
    else:
        print("### PASSED: Memory leak test")
        if os.path.isfile("gc.txt"):
            os.remove("gc.txt")


def get_model_wrapper():

    from model import model

    class MyWrapper(model.ModelWrapper):
        def __init__(self, input):
            super(MyWrapper, self).__init__()
            self.input = input
            self.lag = None
            self.sourceCallback = False
            self.sinkCallback = False

        def SourceCallback(self, buffer):
            print("requesting input...")
            buffer.copy_from(self.input.astype(np.float))
            self.sourceCallback = True

        def LagNotification(self, lag):
            print("Predict is lagging by {}".format(lag))
            self.lag = lag

        def SinkCallback(self, buffer):
            print("output callback happening...")
            self.sinkCallback = True

    from model import model
    print("Input size={}".format(model.get_default_input_shape().Size()))

    input = np.ones((model.get_default_input_shape().Size()))
    return MyWrapper(input)


def simple_test():
    wrapper = get_model_wrapper()
    return wrapper.Predict()


def test_python(model_path, target_dir):
    target_dir = os.path.join(os.path.dirname(model_path), target_dir)

    if os.path.isdir(target_dir):
        rmtree(target_dir)
    os.makedirs(target_dir)

    # invoke "wrap.py" helper to create a compilable C++ project
    wrap_model(model_path, target_dir, "python")

    # compile the project using cmake.
    model_dir = os.path.join(target_dir, "model")
    make_project(model_dir)

    # did it actually build?
    binary_dir = os.path.join(model_dir, "build")
    if os.name == 'nt':
        binary = os.path.join(binary_dir, "release", "_model.pyd")
    else:
        binary = os.path.join(binary_dir, "_model.so")

    if not os.path.isfile(binary):
        print("### wrap_test failed, binary '{}' was not produced".format(os.path.basename(binary)))
        return 1

    # execute the compiled python module and check the output
    sys.path += [target_dir]

    from model import model
    print("Input size={}".format(model.get_default_input_shape().Size()))

    wrapper = get_model_wrapper()
    output = wrapper.Predict()
    time.sleep(1)  # this should cause the lag notification.
    output = wrapper.Predict()

    result = ", ".join([str(x) for x in list(output)])
    print("Prediction={}".format(result))

    if result != "1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0":
        print("### FAILED wrap_test python module did not return the expected results, got: {}".format(result))
        return 1
    else:
        print("### PASSED wrap_test: test_python")

    # make sure callbacks happened
    if not wrapper.sinkCallback:
        print("### FAILED SinkCallback never happened")
        return 1

    # make sure callbacks happened
    if not wrapper.sourceCallback:
        print("### FAILED SourceCallback never happened")
        return 1

    # make sure lag notification was called.
    if wrapper.lag is None:
        print("### FAILED lag notification callback never happened")
        return 1

    # make sure we can call the helper functions.
    try:
        print("GetInputShape={}".format(wrapper.GetInputShape(0)))
        print("GetInputSize={}".format(wrapper.GetInputSize(0)))
        print("GetOutputShape={}".format(wrapper.GetOutputShape(0)))
        print("GetOutputSize={}".format(wrapper.GetOutputSize(0)))
        print("Reset={}".format(wrapper.Reset()))
        print("GetMetadata={}".format(wrapper.GetMetadata("Id")))
    except:
        print("### FAILED wrap_test python failed to call helper methods")
        return 1

    memory_test(simple_test)

    return 0


def test_cpp(model_path, target_path):
    target_dir = os.path.join(os.path.dirname(model_path), target_path)

    if os.path.isdir(target_dir):
        rmtree(target_dir)
    os.makedirs(target_dir)

    copyfile(os.path.join(script_path, "tutorialCMakeLists.txt"),
             os.path.join(target_dir, "CMakeLists.txt"))
    copyfile(os.path.join(script_path, "tutorial.cpp"),
             os.path.join(target_dir, "tutorial.cpp"))

    # invoke "wrap.py" helper to create a compilable C++ project
    wrap_model(model_path, target_dir, "cpp")

    # compile the project using cmake.
    make_project(target_dir)

    # did it actually build?
    binary = os.path.join(target_dir, "build")
    if os.name == 'nt':
        binary = os.path.join(binary, "release", "tutorial.exe")
    else:
        binary = os.path.join(binary, "tutorial")

    if not os.path.isfile(binary):
        print("### wrap_test failed, binary 'tutorial' was not produced")
        return 1

    # execute the compiled tutorial.exe binary and check the output
    cmd = buildtools.EllBuildTools(find_ell.get_ell_root())
    output = cmd.run([binary], print_output=True)
    if "Prediction=1, 2, 3, 4, 5, 6, 7, 8, 9, 10" not in output:
        print("### FAILED: wrap_test cpp binary did not print the expected results, got the following:\n{}"
              .format(output))
        return 1
    else:
        print("### PASSED wrap_test: test_cpp")

    return 0


def test():

    map = create_model(True)

    # save this model.
    model_path = os.path.join(script_path, "model.ell")
    map.Save(model_path)

    rc = test_python(model_path, "tutorial_python")

    rc += test_cpp(model_path, "tutorial_cpp")

    return rc


if __name__ == '__main__':
    test()
