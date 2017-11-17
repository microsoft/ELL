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
        inputShape = predictor.GetInputShape()
        outputShape = predictor.GetOutputShape()

        inputNode = builder.AddInputNode(
            model, inputShape, ELL.ELL_PortType_smallReal)
        nnNode = builder.AddFloatNeuralNetworkPredictorNode(
            model, ELL.ELL_PortElements(inputNode.GetOutputPort("output")), predictor)
        outputNode = builder.AddOutputNode(
            model, outputShape, ELL.ELL_PortElements(nnNode.GetOutputPort("output")))

        ell_map = ELL.ELL_Map(model, ELL.ELL_InputNode(
            inputNode), ELL.ELL_PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map
