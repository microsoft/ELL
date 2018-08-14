import os
import sys
from testing import Testing
import ell
import numpy as np

def test_predictor(testing):
    input_shape = ell.math.TensorShape(1,1,10)
    padding = ell.neural.NoPadding()
    layer_parameters = ell.neural.LayerParameters(input_shape, padding, input_shape, padding, ell.nodes.PortType.smallReal)
    bias = [0,1,2,3,4,5,6,7,8,9]
    layer = ell.neural.BiasLayer(layer_parameters, bias)
    predictor = ell.neural.NeuralNetworkPredictor([layer])
    result = predictor.Predict(np.zeros(10))
    testing.ProcessTest("test_predictor", list(result) == bias)

def test():    
    testing = Testing()
    test_predictor(testing)
    return 0
    
if __name__ == "__main__":
    test()
