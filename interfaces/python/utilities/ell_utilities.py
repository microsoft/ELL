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


def ell_map_from_float_predictor(predictor):
    """Wraps an ell.FloatNeuralNetworkPredictor into an ELL_Map"""

    ell_map = None

    try:
        model = ell.ELL_Model()
        builder = ell.ELL_ModelBuilder()
        inputShape = predictor.GetInputShape()
        outputShape = predictor.GetOutputShape()

        inputNode = builder.AddInputNode(
            model, inputShape, ell.ELL_PortType_smallReal)
        nnNode = builder.AddFloatNeuralNetworkPredictorNode(
            model, ell.ELL_PortElements(inputNode.GetOutputPort("output")), predictor)
        outputNode = builder.AddOutputNode(
            model, outputShape, ell.ELL_PortElements(nnNode.GetOutputPort("output")))

        ell_map = ell.ELL_Map(model, ell.ELL_InputNode(
            inputNode), ell.ELL_PortElements(outputNode.GetOutputPort("output")))

    except:
        print("Error occurrred attempting to wrap ELL predictor in ELL model")
        traceback.print_exc()

    return ell_map
