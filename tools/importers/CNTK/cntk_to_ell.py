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
import math
import time

import numpy as np
from cntk import load_model, ModelFormat
from cntk.ops import softmax

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'lib'))
import find_ell
import ell
from cntk_converters import *
import cntk_layers as cntk_layers
import cntk_utilities as cntk_utilities
import common.importer
import common.memory_shapes as memory_shapes

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
    "BatchNormalization": (CntkStandardConverter, {"mean": (2, "aggregate_mean", "channel"), 
                                                   "variance": (3, "aggregate_variance", "channel"),
                                                   "count": (4, "aggregate_count", "channel"),
                                                   "scale": (0, "scale", "channel"),
                                                   "bias": (1, "bias", "channel")}),
    "ClassificationError": (CntkStandardConverter, {}, "Skip"),
    "Combine": (CntkStandardConverter, {}, "Skip"),
    "Convolution": CntkConvolutionConverter,    
    "CrossEntropyWithSoftmax": CntkCrossEntropyWithSoftmaxConverter,
    "Dense": CntkDenseConverter,
    "Dropout": (CntkStandardConverter, {}, "Passthrough"),
    "ElementTimes": CntkElementTimesConverter,
    "Input": CntkInputConverter,
    "linear": CntkDenseConverter,
    "MaxPooling": CntkPoolingConverter,
    "Minus": (CntkStandardConverter, {"bias": (1, "channel")}),
    "Plus": CntkPlusConverter,
    "PReLU": (CntkStandardConverter, {"alpha": (0, "prelu", "channel_row_column")}, "PReLU"),
    "ReduceElements": (CntkStandardConverter, {}, "Skip"),
    "ReLU": (CntkStandardConverter, {}, "ReLU"),
    "Reshape": CntkReshapeConverter,
    "Softmax": CntkStandardConverter,
    "Splice": CntkSpliceConverter,
    "UserFunction": CntkUserFunctionConverter,
}

def import_nodes(cntk_nodes):
    """
    Returns a common.importer.Model from CNTK nodes.
    """
    importer_model = common.importer.ImporterModel()

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
        predictor = ell.neural.NeuralNetworkPredictor(ellLayers)
    except BaseException as exception:
        _logger.error("Error occurred attempting to convert cntk layers to ELL layers: " + str(exception))
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
        importer_engine = common.importer.ImporterEngine()
        ell_layers = importer_engine.convert(importer_model)
        # Create an ELL neural network predictor from the layers
        predictor = ell.neural.NeuralNetworkPredictor(ell_layers)
    except BaseException as exception:
        _logger.error("Error occurred attempting to convert cntk layers to ELL layers: " + str(exception))
        raise exception

    return predictor

def get_node_output_in_ell_order(cntk_node_results):
    ordered_weights = cntk_node_results
    original_shape = ordered_weights.shape

    if len(original_shape) == 4:
        ordered_weights = ordered_weights.reshape(original_shape[0] * original_shape[1],
            original_shape[2], original_shape[3])
        original_shape = ordered_weights.shape

    if len(original_shape) == 3:
        ordered_weights = np.moveaxis(ordered_weights, 0, -1)
        ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
            original_shape[1], original_shape[2], original_shape[0])
    elif len(original_shape) == 2:
        ordered_weights = np.moveaxis(ordered_weights, 0, -1)
        ordered_weights = ordered_weights.ravel().astype(
            np.float).reshape(original_shape[1], 1, original_shape[0])
    elif len(original_shape) == 1:
        ordered_weights = ordered_weights.ravel().astype(
            np.float).reshape(1, 1, original_shape.size)
    else:
        raise NotImplementedError(
            "Unsupported tensor dimensions {}".format(len(original_shape)))

    return ordered_weights

def print_comparison(tensorA, tensorB):
    pass
    for row in range(tensorA.shape[0]):
        for column in range(tensorA.shape[1]):
            for channel in range(tensorA.shape[2]):
                if math.fabs(tensorA[row, column, channel] - tensorB[row, column, channel]) > 0.0001:
                    print("A[{},{},{}] = {}".format(row, column, channel, tensorA[row, column, channel]))
                    print("B[{},{},{}] = {}".format(row, column, channel, tensorB[row, column, channel]))

def verify_ell_nodes_in_vision_model(ell_map, cntk_model, cntk_nodes, ordered_importer_nodes, node_mapping, testing_info):
    _logger.info("\n\nVerification of model nodes starting")

    cntk_node_results = None
    try:
        # Get input to the CNTK model
        cntk_input_tensor = np.random.random((cntk_model.arguments[0].shape)).astype(np.float32) * 255
        ell_input_tensor = memory_shapes.get_tensor_in_ell_order(cntk_input_tensor, "channel_row_column").ravel().astype(np.float32)

        # For convenient lookup, map from the cntk intermediate node to the
        # importer node
        cntk_nodes_map = {}
        for cntk_node in cntk_nodes:
            cntk_nodes_map[cntk_node.uid] = cntk_node

        # Feed input to the ELL model
        _logger.info("Getting computed ELL results")
        ell_map.Compute(ell_input_tensor, dtype=np.float32)

        # Walk list of importer nodes
        for importer_node in ordered_importer_nodes:
            if importer_node.operation_type in ["Input", "Passthrough", "Reshape", "Skip", "Softmax"]:
                if importer_node.operation_type == "Softmax":
                    testing_info["apply_softmax"] = True
                continue
            _logger.info("Looking at node: {}".format(importer_node))


            # Get the CNTK output values
            cntk_node = cntk_nodes_map[importer_node.id]
            try:
                if cntk_node.op_name != "UserFunction":
                    clone = cntk_node.clone(CloneMethod.clone)
            except BaseException as e:
                _logger.info("Couldn't clone {}, skipping".format(cntk_node.uid))
                continue

            # Get output from CNTK model
            _logger.info("Getting CNTK results")
            if (len(clone.arguments) > 1):
                arg1_output = np.zeros(clone.arguments[1].shape).astype(np.float32)
                cntk_node_results = clone.eval({clone.arguments[0]:[cntk_input_tensor], clone.arguments[1]:arg1_output})
            else:
                cntk_node_results = clone.eval({clone.arguments[0]:[cntk_input_tensor]})
            # Reorder cntk node output
            cntk_node_results = get_node_output_in_ell_order(cntk_node_results)
            # Get the results from the last ELL node for that group
            ell_node = node_mapping[importer_node.id][-1]
            ell_node_output_port = ell_node.GetOutputPort("output")
            ell_node_results = np.zeros((ell_node_output_port.Size(),), dtype=np.float32)
            for i in range(ell_node_output_port.Size()):
                ell_node_results[i] = ell_node_output_port.GetDoubleOutput(i)
            
            output_shape = cntk_node_results.shape
            if (len(output_shape) == 3):
                padding = importer_node.output_padding["size"]
                output_shape_with_padding = (output_shape[0] + 2*padding,
                                             output_shape[1] + 2*padding,
                                             output_shape[2])
                ell_node_results = ell_node_results.reshape(output_shape_with_padding)
                # Remove padding and look at active region only
                ell_node_results = ell_node_results[padding:output_shape[0]+padding,padding:output_shape[1]+padding,:]

            # Compare results. Some layers have large numbers (e.g > 500.734) and some small numbers
            # (e.g. 0.0038453). To make the comparison more resilient and meaningful for large numbers,
            # normalize before comparing, since comparison is being done on significant digits.
            max = cntk_node_results.max()
            if max > 100:
                cntk_node_results = cntk_node_results / max
                ell_node_results = ell_node_results / max

            np.testing.assert_allclose(
                cntk_node_results, ell_node_results, rtol=1e-04, atol=1e-04, err_msg=(
                    'results for compiled ELL model do not match CNTK output!'))

            _logger.info("Output for {} verified\n".format(importer_node.id))

    except BaseException as exception:
        _logger.error("Verification of model output failed")
        #if cntk_node_results is not None:
        #    print_comparison(cntk_node_results, ell_node_results)
        raise exception
    _logger.info("Verification of model nodes complete\n")

def verify_ell_output_in_vision_model(ell_map, cntk_model, testing_info):
    _logger.info("Verification of model output starting")
    try:
        cntk_input_tensor = np.random.random((cntk_model.arguments[0].shape)).astype(np.float32) * 255
        ell_input_tensor = memory_shapes.get_tensor_in_ell_order(cntk_input_tensor, "channel_row_column").ravel().astype(np.float32)

        # Get output from CNTK model
        _logger.info("Getting CNTK results")
        if (len(cntk_model.arguments) > 1):
            arg1_output = np.zeros(cntk_model.arguments[1].shape).astype(np.float32)
            cntk_output = cntk_model.eval({cntk_model.arguments[0]:[cntk_input_tensor], cntk_model.arguments[1]:arg1_output})
        else:
            cntk_output = cntk_model.eval({cntk_model.arguments[0]:[cntk_input_tensor]})
        size = 0
        if isinstance(cntk_output,dict):
            for key in cntk_model.outputs:
                shape = key.shape
                if len(shape) > 0:
                    s = np.max(shape)
                    if (s > size):
                        size = s
                        cntk_output = cntk_output[key][0]
        else:
            cntk_output = cntk_output[0]
        # Check whether softmax needs to be applied or not.
        if testing_info["apply_softmax"]:
            cntk_output = softmax(cntk_output).eval()

        # Get computed ELL result
        _logger.info("Getting computed ELL results")
        result_from_compute = np.array(ell_map.Compute(ell_input_tensor, dtype=np.float32))

        # Get compiled ELL result
        _logger.info("Getting compiled ELL results")
        compiler_options = ell.model.MapCompilerOptions()
        compiler_options.useBlas = True
        compiled_ell_map = ell_map.Compile("host", "model", "predict", compilerOptions=compiler_options, dtype=np.float32)

        result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))

        # Verify the computed result against the cntk result
        np.testing.assert_array_almost_equal(
           cntk_output, result_from_compute, decimal=4, err_msg=(
               'results for computed ELL model do not match CNTK output!'))
        _logger.info("Verified computed result against CNTK")
            
        # Verify the compiled result  against the cntk result
        np.testing.assert_array_almost_equal(
            cntk_output, result_from_compiled, decimal=4, err_msg=(
                'results for compiled ELL model do not match CNTK output!'))
        _logger.info("Verified compiled result against CNTK")
        
        # Verify the compiled result agrees with the computed result
        np.testing.assert_array_almost_equal(
            result_from_compute, result_from_compiled, decimal=4, err_msg=(
                'results for computed ELL model do not match results from compiled ELL model!'))
        _logger.info("Verified compiled result against computed result")

        # Get timing info
        total_time = 0
        num_frames = 50
        _logger.info("Sending {} frames through model...".format(num_frames))
        for i in range(num_frames):
            cntk_input_tensor = np.random.random((cntk_model.arguments[0].shape)).astype(np.float32) * 255
            ell_input_tensor = memory_shapes.get_tensor_in_ell_order(cntk_input_tensor, "channel_row_column").ravel().astype(np.float32)
            start = time.time()
            result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))
            end = time.time()
            total_time += end - start
        total_time /= num_frames
        _logger.info("Average speed: {:.0f}ms/frame".format(total_time * 1000))
    except BaseException as exception:
        _logger.error("Verification of model output failed")
        raise exception

    _logger.info("Verification of model output complete")

def map_from_cntk_model_using_new_engine(modelFile, step_interval_msec=0, lag_threshold_msec=0, plot_model=False, verify_model={"audio": False, "vision": False}):
    """
    Loads a CNTK model and returns an ell.model.Map
    """

    _logger.info("Loading...")
    cntk_model = load_model(modelFile)
    _logger.info("\nFinished loading.")

    if plot_model:
        filename = os.path.join(os.path.dirname(modelFile), os.path.basename(modelFile) + ".svg")
        Utilities.plot_model(cntk_model, filename)

    try:
        _logger.info("Pre-processing...")
        # Get the relevant nodes from CNTK that make up the model
        cntk_nodes = Utilities.get_model_nodes(cntk_model)
        _logger.info("\nFinished pre-processing.")

        # Create an ImporterModel from the CNTK nodes
        importer_model = import_nodes(cntk_nodes)
        # Use the common importer engine to drive conversion of the 
        # ImporterModel to ELL layers
        importer_engine = common.importer.ImporterEngine(step_interval_msec=step_interval_msec, lag_threshold_msec=lag_threshold_msec)
        ell_map = importer_engine.convert_nodes(importer_model)

    except BaseException as exception:
        _logger.error("Error occurred attempting to convert cntk layers to ELL model using nodes")
        raise exception

    if verify_model["audio"]:
        _logger.info("Verification of audio models is not supported at this time, skipping verification")
    elif verify_model["vision"]:
        testing_info = {"apply_softmax": False }
        try:
            ordered_importer_nodes, node_mapping = importer_engine.get_importer_node_to_ell_mapping()
            verify_ell_nodes_in_vision_model(ell_map, cntk_model, cntk_nodes, ordered_importer_nodes, node_mapping, testing_info)
            verify_ell_output_in_vision_model(ell_map, cntk_model, testing_info)            
        except BaseException as exception:
            _logger.error("Error occurred verifying imported model")
            basename, ext = os.path.splitext(modelFile)
            ell_map.Save(basename + ".ell.verification_failed")
            raise exception

    return ell_map
