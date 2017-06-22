####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     step_test.py (examples)
# Authors:  Lisa Ong
#
# Requires: Python 3.x
#
####################################################################################################
import sys

sys.path.append('.')
sys.path.append('./..')
sys.path.append('./../Release')
sys.path.append('./../Debug')

from testing import Testing

# import the emitted model
import ELL_step10 as model

count = 0


def emitted_callback(data):
    global count

    for i in range(0, 10):
        data[i] = i
    count = count + 1
    return True


def test_emitted_model(testing):
    # python somehow needs to know about the data vector type, so we provide it
    buffer = model.DoubleVector(10)

    # calling pattern is a bit unnatural
    predictor = model.ELL_step10Predictor.get_instance(
        emitted_callback, buffer)

    # step returns the result of the last call to compute
    result = predictor.step()
    for i in range(0, 10):
        testing.ProcessTest(
            "Testing Step result values", testing.IsEqual(result[i], 2 * buffer[i]))
    testing.ProcessTest(
        "Testing Step callback count", testing.IsEqual(count, 1))

    # call the one-shot predict function
    # buffer is already filled by the callback, so we reuse it for convenience
    result_oneshot = model.DoubleVector(10)
    model.step_10_predict(buffer, result_oneshot)
    for i in range(0, 10):
        testing.ProcessTest(
            "Testing Step result values", testing.IsEqual(result_oneshot[i], 2 * buffer[i]))


def test():
    testing = Testing()
    test_emitted_model(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0


if __name__ == '__main__':
    test()
