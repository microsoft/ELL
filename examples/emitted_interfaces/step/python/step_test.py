import sys

sys.path.append('.')
sys.path.append('./..')
sys.path.append('./../Release')
sys.path.append('./../Debug')

from testing import Testing

# import the emitted model
import ELL_step10


def emitted_callback(data):
    for i in range(0, 10):
        data[i] = i
    return True


def test_emitted_model(testing):
    # python somehow needs to know about the data vector type, so we provide it
    buffer = ELL_step10.DoubleVector(10)

    # calling pattern is a bit unnatural
    predictor = ELL_step10.Step10Predictor.GetInstance(
        emitted_callback, buffer)

    # step returns the result of the last call to compute
    result = predictor.Step()
    for i in range(0, 10):
        testing.ProcessTest(
            "Testing Step", testing.IsEqual(result[i], 2 * buffer[i]))


def test():
    testing = Testing()
    test_emitted_model(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0


if __name__ == '__main__':
    test()
