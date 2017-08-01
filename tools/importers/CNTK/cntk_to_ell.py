####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_to_ell.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################
import ELL
import ell_utilities
import numpy as np
from cntk import load_model

# Importer
import lib.cntk_layers as cntk_layers
import lib.cntk_utilities as cntk_utilities


def predictor_from_cntk_model(modelFile):
    """Loads a CNTK model and returns an ELL.NeuralNetworkPredictor"""

    print("Loading...")
    z = load_model(modelFile)
    print("\nFinished loading.")

    print("Pre-processing...")
    modelLayers = cntk_utilities.get_model_layers(z)

    # Get the relevant CNTK layers that we will convert to ELL
    layersToConvert = cntk_layers.get_filtered_layers_list(modelLayers)
    print("\nFinished pre-processing.")

    predictor = None

    try:
        # Create a list of ELL layers from the CNTK layers
        ellLayers = cntk_layers.convert_cntk_layers_to_ell_layers(
            layersToConvert)
        # Create an ELL neural network predictor from the layers
        predictor = ELL.FloatNeuralNetworkPredictor(ellLayers)

    except:
        import traceback
        print("Error occurred attempting to convert cntk layers to ELL layers")
        traceback.print_exc()

    return predictor
