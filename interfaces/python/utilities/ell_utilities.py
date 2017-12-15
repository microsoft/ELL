####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     ell_utilities.py (interfaces)
# Authors:  Lisa Ong
#
# Requires: Python 3.x
#
####################################################################################################
import ell
import sys
import traceback


def ell_map_from_float_predictor(predictor, step_interval_msec=0, lag_threshold=5,
    function_prefix=""):
    """Wraps an ell.FloatNeuralNetworkPredictor into an ell.model.Map

    Optional parameters:
       step_interval_msec - when set, creates an Map that is steppable for
                            an interval
       lag_threshold - the number of intervals to fall behind before sending a lag 
                       callback (applies only to a steppable Map)
       function_prefix - the prefix for function names
    """

    ell_map = None

    try:
        model = ell.model.Model()
        builder = ell.model.ModelBuilder()
        inputShape = predictor.GetInputShape()
        outputShape = predictor.GetOutputShape()
        inputNode = None
        outputNode = None

        if function_prefix:
            function_prefix = function_prefix + '_'

        if step_interval_msec:
            inputNode = builder.AddInputNode(
                model, ell.math.TensorShape(1, 1, 1), ell.nodes.PortType.real)
            clockNode = builder.AddClockNode(
                model, ell.nodes.PortElements(inputNode.GetOutputPort("output")),
                float(step_interval_msec), lag_threshold,
                "{}LagNotification".format(function_prefix))
            sourceNode = builder.AddSourceNode(
                model, ell.nodes.PortElements(clockNode.GetOutputPort("output")),
                ell.nodes.PortType.smallReal, inputShape,
                "{}InputCallback".format(function_prefix))
            nnNode = builder.AddFloatNeuralNetworkPredictorNode(
                model, ell.nodes.PortElements(sourceNode.GetOutputPort("output")),
                predictor)
            sinkNode = builder.AddSinkNode(
                model, ell.nodes.PortElements(nnNode.GetOutputPort("output")),
                outputShape,
                "{}OutputCallback".format(function_prefix))
            outputNode = builder.AddOutputNode(
                model, outputShape, ell.nodes.PortElements(sinkNode.GetOutputPort("output")))
        else:
            inputNode = builder.AddInputNode(
                model, inputShape, ell.nodes.PortType.smallReal)
            nnNode = builder.AddFloatNeuralNetworkPredictorNode(
                model, ell.nodes.PortElements(inputNode.GetOutputPort("output")),
                predictor)
            outputNode = builder.AddOutputNode(
                model, outputShape, ell.nodes.PortElements(nnNode.GetOutputPort("output")))

        ell_map = ell.model.Map(model, ell.nodes.InputNode(
            inputNode), ell.nodes.PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map
