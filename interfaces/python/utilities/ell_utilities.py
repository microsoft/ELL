####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     ell_utilities.py (interfaces)
# Authors:  Lisa Ong
#
# Requires: Python 3.x
#
####################################################################################################
import ELL
import sys
import traceback

def ell_map_from_float_predictor(predictor):
    """Wraps an ELL.FloatNeuralNetworkPredictor into an ELL.Map"""

    ell_map = None

    try:
        model = ELL.ELL_Model()
        builder = ELL.ELL_ModelBuilder()
        shape = predictor.GetInputShape()

        inputNode = builder.AddInputNode(
            model, shape.rows * shape.columns * shape.channels, ELL.ELL_PortType_smallReal)
        nnNode = builder.AddFloatNeuralNetworkPredictorNode(
            model, ELL.ELL_PortElements(inputNode.GetOutputPort("output")), predictor)
        outputNode = builder.AddOutputNode(
            model, ELL.ELL_PortElements(nnNode.GetOutputPort("output")))

        ell_map = ELL.ELL_Map(model, ELL.ELL_InputNode(
            inputNode), ELL.ELL_PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map


def ell_steppable_map_from_float_predictor(predictor, millisecond_interval, input_callback_name, output_callback_name):
    """Wraps an ELL.FloatNeuralNetworkPredictor into an ELL.SteppableMap"""

    ell_steppable_map = None

    try:
        model = ELL.ELL_Model()
        builder = ELL.ELL_ModelBuilder()
        shape = predictor.GetInputShape()

        inputNode = builder.AddInputNode(
            model, 2, ELL.ELL_PortType_real)  # time signal
        sourceNode = builder.AddSourceNode(
            model, ELL.ELL_PortElements(inputNode.GetOutputPort("output")),
            ELL.ELL_PortType_smallReal, shape.rows * shape.columns * shape.channels, input_callback_name)
        nnNode = builder.AddFloatNeuralNetworkPredictorNode(
            model, ELL.ELL_PortElements(sourceNode.GetOutputPort("output")), predictor)
        sinkNode = builder.AddSinkNode(
            model, ELL.ELL_PortElements(nnNode.GetOutputPort("output")), output_callback_name)
        outputNode = builder.AddOutputNode(
            model, ELL.ELL_PortElements(sinkNode.GetOutputPort("output")))

        ell_steppable_map = ELL.ELL_SteppableMap(
            model, ELL.ELL_InputNode(inputNode),
            ELL.ELL_PortElements(outputNode.GetOutputPort("output")),
            ELL.ELL_ClockType_steadyClock, millisecond_interval)

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_steppable_map
