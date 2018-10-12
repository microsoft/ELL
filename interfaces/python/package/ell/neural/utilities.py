####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     ell_utilities.py (interfaces)
# Authors:  Lisa Ong, Chris Lovett
#
# Requires: Python 3.x
#
####################################################################################################
import ell
import sys
import re
import traceback

def ell_map_from_predictor(predictor, step_interval_msec=None, lag_threshold_msec=None,
    function_prefix=""):
    
    """Wraps an ell.NeuralNetworkPredictor into an ell.model.Map

    Optional parameters:
       step_interval_msec - step interval for the model (None for non-steppable model)
       lag_threshold_msec - how long to fall behind before sending a lag callback
                            (applies when step_interval_msec > 0)
       function_prefix    - the prefix for function names
    """
    
    ell_map = None
    portType = predictor.GetDataType()

    try:
        model = ell.model.Model()
        builder = ell.model.ModelBuilder()
        inputShape = predictor.GetInputShape()
        outputShape = predictor.GetOutputShape()
        inputNode = None
        outputNode = None

        if function_prefix:
            if not re.match(r"^[a-zA-Z_][a-zA-Z0-9_]*$", function_prefix):
                raise Exception("function_prefix is not a valid python Name: " + function_prefix)
            function_prefix = function_prefix + '_'

        originalInputNode = None
        if step_interval_msec:
            # in the steppable case the input is a clock ticks (which is a double)
            inputNode = builder.AddInputNode(
                model, ell.math.TensorShape(1, 1, 1), ell.nodes.PortType.real)
            originalInputNode = inputNode
            clockNode = builder.AddClockNode(
                model, ell.nodes.PortElements(inputNode.GetOutputPort("output")),
                float(step_interval_msec), float(lag_threshold_msec),
                "{}LagNotification".format(function_prefix))
            sourceNode = builder.AddSourceNode(
                model, ell.nodes.PortElements(clockNode.GetOutputPort("output")),
                portType, inputShape,
                "{}InputCallback".format(function_prefix))
            inputNode = sourceNode
        else:
            inputNode = builder.AddInputNode(
                model, inputShape, portType)
            originalInputNode = inputNode

        nnNode = builder.AddNeuralNetworkPredictorNode(
            model, ell.nodes.PortElements(inputNode.GetOutputPort("output")),
            predictor)

        output = nnNode
        if step_interval_msec:
            # add a sink node that always triggers
            sinkNode = builder.AddSinkNode(
                model, ell.nodes.PortElements(output.GetOutputPort("output")),
                outputShape,
                "{}OutputCallback".format(function_prefix))
            output = sinkNode

        outputNode = builder.AddOutputNode(
            model, outputShape, ell.nodes.PortElements(output.GetOutputPort("output")))

        ell_map = ell.model.Map(model, originalInputNode, 
            ell.nodes.PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map

    
def ell_map_from_model(model, model_input_node, input_shape, model_output_node, output_shape, step_interval_msec=None, lag_threshold_msec=None,
    function_prefix=""):
    """Wraps an ell.model.MOdel in a ell.model.Map

    Optional parameters:
       step_interval_msec - step interval for the model (None for non-steppable map)
       lag_threshold_msec - how long to fall behind before sending a lag 
                            callback (applies when step_interval_msec > 0)
       function_prefix - the prefix for function names
    """
    
    ell_map = None

    try:
        builder = ell.model.ModelBuilder()
        inputShape = input_shape
        outputShape = output_shape
        inputNode = model_input_node
        outputNode = model_output_node

        if function_prefix:
            if not re.match(r"^[a-zA-Z_][a-zA-Z0-9_]*$", function_prefix):
                raise Exception("function_prefix is not a valid python Name: " + prefix)
            function_prefix = function_prefix + '_'

        portType = inputNode.GetOutputPort("output").GetOutputType()
        
        originalInputNode = inputNode
        if step_interval_msec:
            
            clockNode = builder.AddClockNode(
                model, ell.nodes.PortElements(inputNode.GetOutputPort("output")),
                float(step_interval_msec), float(lag_threshold_msec),
                "{}LagNotification".format(function_prefix))
            sourceNode = builder.AddSourceNode(
                model, ell.nodes.PortElements(clockNode.GetOutputPort("output")),
                portType, inputShape,
                "{}InputCallback".format(function_prefix))
            # add a sink node that always triggers
            sinkNode = builder.AddSinkNode(
                model, ell.nodes.PortElements(outputNode.GetOutputPort("output")),
                outputShape,
                "{}OutputCallback".format(function_prefix))
            outputNode = sinkNode
            
        ell_map = ell.model.Map(model, originalInputNode, 
            ell.nodes.PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map
