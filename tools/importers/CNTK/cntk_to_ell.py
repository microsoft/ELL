####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_to_ell.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.4
#
####################################################################################################

import sys
import os
import logging

import numpy as np
from cntk import load_model

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import ell
from lib.cntk_converters import *
import lib.cntk_layers as cntk_layers
import lib.cntk_utilities as cntk_utilities
import lib.shared_importer as shared_importer

_logger = logging.getLogger(__name__)

#
# Mapping of CNTK operation type to the converter object to use.
# For each entry:
# key - the name of the CNTK operation (the op_name of the node)
# value - converter class to use, or,
#         tuple(converter_class, weight_mapping, operation_type)
#           When a tuple is used, the convert class is initialized with:
#               converter_type(weight_mapping, operation_type)
#               or,
#               converter_type(weight_mapping)
#               if operation_type is missing. 
# e.g.
# cntk_converter_map =
# {
#     "Activation" = CntkStandardConverter
#     "AveragePooling" = CntkPoolingConverter
#     "BatchNormalization" = (CntkStandardConverter,{"mean": 3, "variance": 4, "count": 5, "scale": 0, "bias": 1})
#     ...
# }
# For "Activation":
#   - the CntkStandardConverter will be used to create an ImporterNode with 
#     operation_type "Activation"
#   - standard inputs, outputs, input/output shapes etc. derived from CNTK node
#     inputs/outputs.
# For "AveragePooling":
#   - the CntkPoolingConverter class will be used to create the corresponding ImporterNode
# For "BatchNormalization":
#   - the CntkStandardConverter will create an ImporterNode with 
#     operation_type "BatchNormalization"
#   - standard inputs, outputs, input/output shapes etc. derived from CNTK node
#     inputs/outputs.
#   - Weights will be retrieved from the inputs, with "mean" coming from index 3, 
#     "variance from index 4" etc.
#
cntk_converter_map = {
    "Activation": CntkStandardConverter,
    "AveragePooling": CntkPoolingConverter,
    "BatchNormalization": (CntkStandardConverter, {"mean": (2, "channel"), "variance": (3, "channel"), "count": 4, "scale": (0, "channel"), "bias": (1, "channel")}),
    "ClassificationError": (CntkStandardConverter, {}, "Skip"),
    "Combine": (CntkStandardConverter, {}, "Skip"),
    "Convolution": CntkConvolutionConverter,    
    "Dense": CntkDenseConverter,
    "Dropout": (CntkStandardConverter, {}, "Passthrough"),
    "ElementTimes": CntkElementTimesConverter,
    "Input": CntkInputConverter,
    "linear": CntkDenseConverter,
    "MaxPooling": CntkPoolingConverter,
    "Minus": (CntkStandardConverter, {"bias": (1, "channel")}),
    "Splice": CntkSpliceConverter,
    "ReduceElements": (CntkStandardConverter, {}, "Skip"),
    "Reshape": CntkReshapeConverter,
    "CrossEntropyWithSoftmax": CntkCrossEntropyWithSoftmaxConverter,
}

def import_nodes(cntk_nodes):
    """
    Returns a shared_importer.Model from CNTK nodes.
    """
    importer_model = shared_importer.ImporterModel()

    for cntk_node in cntk_nodes:
        node = CntkConverterFactory.get_converter(cntk_node, cntk_converter_map)
        if node:
            importer_model.add_node(node.uid, node.convert())
            for w in node.weights:
                weight = node.weights[w]
                importer_model.add_tensor(weight[0], weight[1], weight[2])

    return importer_model

def predictor_from_cntk_model(modelFile, plotModel=False):
    """Loads a CNTK model and returns an ell.neural.NeuralNetworkPredictor"""

    _logger.info("Loading...")
    z = load_model(modelFile)
    _logger.info("\nFinished loading.")

    if plotModel:
        filename = os.path.join(os.path.dirname(modelFile), os.path.basename(modelFile) + ".svg")
        cntk_utilities.plot_model(z, filename)

    _logger.info("Pre-processing...")
    modelLayers = cntk_utilities.get_model_layers(z)

    # Get the relevant CNTK layers that we will convert to ELL
    layersToConvert = cntk_layers.get_filtered_layers_list(modelLayers)
    _logger.info("\nFinished pre-processing.")

    predictor = None

    try:
        # Create a list of ELL layers from the CNTK layers
        ellLayers = cntk_layers.convert_cntk_layers_to_ell_layers(
            layersToConvert)
        # Create an ELL neural network predictor from the layers
        predictor = ell.neural.FloatNeuralNetworkPredictor(ellLayers)
    except BaseException as exception:
        _logger.error("Error occurred attempting to convert cntk layers to ELL layers")
        raise exception

    return predictor

def predictor_from_cntk_model_using_new_engine(modelFile, plotModel=True):
    """
    Loads a CNTK model and returns an ell.neural.NeuralNetworkPredictor
    """

    _logger.info("Loading...")
    z = load_model(modelFile)
    _logger.info("\nFinished loading.")

    if plotModel:
        filename = os.path.join(os.path.dirname(modelFile), os.path.basename(modelFile) + ".svg")
        Utilities.plot_model(z, filename)

    try:
        _logger.info("Pre-processing...")
        # Get the relevant nodes from CNTK that make up the model
        importer_nodes = Utilities.get_model_nodes(z)
        _logger.info("\nFinished pre-processing.")

        # Create an ImporterModel from the CNTK nodes
        importer_model = import_nodes(importer_nodes)
        # Use the common importer engine to drive conversion of the 
        # ImporterModel to ELL layers
        importer_engine = shared_importer.ImporterEngine()
        ell_layers = importer_engine.convert(importer_model)
        # Create an ELL neural network predictor from the layers
        predictor = ell.neural.FloatNeuralNetworkPredictor(ell_layers)
    except BaseException as exception:
        _logger.error("Error occurred attempting to convert cntk layers to ELL layers")
        raise exception

    return predictor
