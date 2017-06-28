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

SkipTests = False
try:
    sys.path.append('.')
    sys.path.append('./..')
    sys.path.append('./../Release')
    sys.path.append('./../Debug')
    from testing import Testing
    # import the emitted model
    import ELL_step10 as model
    source_callback_count = 0
    sink_callback_count = 0
    sink_result = model.DoubleVector(10)
except Exception:
    SkipTests = True
    source_callback_count = 0
    sink_callback_count = 0
    sink_result = []

# source callback for data
def data_callback(data):
    global source_callback_count

    for i in range(0, 10):
        data[i] = i
    source_callback_count = source_callback_count + 1
    # returning True because we have data
    return True


# sink callback for results
def results_callback(data):
    global sink_callback_count
    global sink_result
    sink_result = data
    sink_callback_count = sink_callback_count + 1


def test_emitted_model(testing):
    # python somehow needs to know about the data vector type, so we provide it
    buffer = model.DoubleVector(10)

    predictor = model.ELL_step10Predictor.get_instance(
        data_callback, buffer, results_callback)

    # setup expected result
    expected = model.DoubleVector(10)
    for i in range(0, 10):
        expected[i] = i * 2
    expected_callback_count = 1

    # step returns the result of the last call to compute
    result = predictor.step()

    wait_time = predictor.wait_time_for_next_step()
    interval = predictor.get_interval()
    testing.ProcessTest(
        "Testing wait_time_for_next_step and get_interval", wait_time <= interval)

    testing.ProcessTest("Testing step result values",
                        testing.IsEqual(result, expected))
    testing.ProcessTest("Testing source callback count",
                        testing.IsEqual(source_callback_count, expected_callback_count))
    testing.ProcessTest("Testing sink callback count",
                        testing.IsEqual(sink_callback_count, expected_callback_count))
    testing.ProcessTest("Testing sink callback results",
                        testing.IsEqual(sink_result, expected))

    # call the one-shot predict function
    oneshot_result = model.DoubleVector(10)
    time_signal = model.DoubleVector(2)
    model.step_10_predict(time_signal, oneshot_result)
    testing.ProcessTest("Testing one-shot result values",
                        testing.IsEqual(oneshot_result, expected))

def test():
    testing = Testing()
    test_emitted_model(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0


if __name__ == '__main__':
    if not SkipTests:
        test()
