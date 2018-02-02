#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     wrap_test.py
##  Authors:  Chris Lovett, Kern Handa
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
from shutil import copyfile, rmtree
import numpy as np

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [ os.path.join(script_path, '..', '..', '..', 'tools', 'utilities', 
              'pythonlibs')]
import find_ell
import ell

ell_build_dir = find_ell.find_ell_build()
sys.path += [ os.path.join(ell_build_dir, "tools", "wrap") ]
import wrap
import buildtools

def wrap_model(model, target_dir, language):
    builder = wrap.ModuleBuilder()
    args = [ model, 
            "--outdir", os.path.join(target_dir, "model"), 
            "--language", language, 
            "--target", "host", 
            "--module_name", "model" ]
    builder.parse_command_line(args)
    builder.run()

def make_project(target_dir):

    build_dir = os.path.join(target_dir, "build")
    if os.path.isdir(build_dir):
        rmtree(build_dir)
    os.makedirs(build_dir)
        
    current_path = os.getcwd()
    os.chdir(build_dir)
    cmd = buildtools.EllBuildTools(find_ell.get_ell_root(), verbose=True)
    cmake = [ "cmake", ".."]
    if os.name == 'nt':
        cmake = [ "cmake", "-G", "Visual Studio 15 2017 Win64", ".."]
    cmd.run(cmake, print_output=True)

    make = [ "make" ]
    if os.name == 'nt':
        make = [ "cmake", "--build", ".", "--config", "Release" ]
    cmd.run(make, print_output=True)
    os.chdir(current_path)

def create_model():
    
    model = ell.model.Model()
    mb = ell.model.ModelBuilder()
    
    shape = ell.math.TensorShape(1,1,10)

    # add node representing input (input nodes have no input, they are the input)
    # the OutputPort of the InputNode is the input data they pass along to the next node
    inputNode = mb.AddInputNode(model, ell.math.TensorShape(1, 1, 1), ell.nodes.PortType.real)
    inputLink = inputNode.GetOutputPort("output")

    # clock node is required to setup the timing of the callbacks.
    clockNode = mb.AddClockNode(model, ell.nodes.PortElements(inputLink), float(0), float(10),
        "LagNotification")
    clockLink = clockNode.GetOutputPort("output")

    # add a SourceNode that gets input from the application
    sourceNode = mb.AddSourceNode(
        model, ell.nodes.PortElements(clockLink),
        ell.nodes.PortType.real, shape, "InputCallback")
    sourceLink = sourceNode.GetOutputPort("output")
        
    # add a constant vector to the input provided in the InputCallback
    constNode = mb.AddConstantNode(model, [ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 ], 
            ell.nodes.PortType.real)
    constLink = constNode.GetOutputPort("output")

    addNode = mb.AddBinaryOperationNode(model, 
                    ell.nodes.PortElements(sourceLink), 
                    ell.nodes.PortElements(constLink), 
                    ell.nodes.BinaryOperationType.add)    
    addLink = addNode.GetOutputPort("output")

    # add a SinkNode to send this output to the application via OutputCallback
    # (setup a condition for the sink node that is always true).
    conditionNode = mb.AddConstantNode(
        model, ell.math.DoubleVector([1.0]), ell.nodes.PortType.boolean)
    conditionNodeLink = conditionNode.GetOutputPort("output")
    sinkNode = mb.AddSinkNode(model, ell.nodes.PortElements(addLink),                              
            ell.nodes.PortElements(conditionNodeLink),
        shape, "OutputCallback")
    sinkLink = sinkNode.GetOutputPort("output")
        
    # add a node representing output from the model.
    outputNode = mb.AddOutputNode(model, shape, ell.nodes.PortElements(sinkLink))
    outputResult = outputNode.GetOutputPort("output")

    # create Map that wraps the model.
    map = ell.model.Map(model, inputNode, ell.nodes.PortElements(outputResult))
    return map

def test_python(model_path):    
    target_dir = os.path.join(os.path.dirname(model_path), "tutorial_python")
    
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
    sys.path += [ target_dir ]

    from model import model

    input = np.zeros((model.get_default_input_shape().Size()))
    output = model.predict(input)
    result = ", ".join([str(x) for x in list(output)])

    if result != "0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0":
        print("### FAILED wrap_test python module did not return the expected results, got: {}".format(result))
        return 1
    else:
        print("### PASSED wrap_test: test_python")

    return 0
    

def test_cpp(model_path):    
    target_dir = os.path.join(os.path.dirname(model_path), "tutorial_cpp")
    
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
    cmd = buildtools.EllBuildTools(find_ell.get_ell_root(), verbose=True)
    output = cmd.run([binary], print_output=True)
    if not "Prediction=0, 1, 2, 3, 4, 5, 6, 7, 8, 9" in output:
        print("### FAILED: wrap_test cpp binary did not print the expected results, got the following:\n{}".format(output))
        return 1
    else:
        print("### PASSED wrap_test: test_cpp")
    
    return 0

def test():
    
    map = create_model()
    
    # save this model.
    model_path = os.path.join(script_path, "model.ell")  
    map.Save(model_path)

    rc = test_python(model_path)

    rc += test_cpp(model_path)

    return rc
    

if __name__ == '__main__':    
    test()

