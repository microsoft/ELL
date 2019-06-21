import json
import math
import os
from testing import Testing
import numpy as np
import ell_helper  # noqa: F401
import ell

script_path = os.path.dirname(os.path.abspath(__file__))


def test_with_serialization(testing, map, test_name, callback):
    for i in range(3):
        if i > 0:
            filename = "{}{}.json".format(test_name, i)
            map.Save(filename)
            try:
                with open(filename, "r") as f:
                    json.load(f)
            except Exception as e:
                raise Exception("### ELL model is not valid json: {}".format(e))
            map = ell.model.Map(filename)

        callback(testing, map, i)


def test_reorder(testing):
    # Test a model that reorders the input data
    model = ell.model.Model()
    x = np.array([1, 2, 3, 4, 5, 6]).reshape((2, 1, 3)).astype(np.float32)
    order = [2, 0, 1]
    expected = np.transpose(x, tuple(order))
    layout = ell.model.PortMemoryLayout(list(x.shape))
    input = model.AddInput(layout, ell.nodes.PortType.real)
    reorder = model.AddReorderData(input, order)
    layout = ell.model.PortMemoryLayout(list(expected.shape))
    output = model.AddOutput(layout, reorder)
    map = ell.model.Map(model, input, output)
    result = map.Compute(x)
    testing.ProcessTest("Testing ReorderDataNode Compute",
                        testing.IsEqual(np.array(result), expected.ravel()))
    compiled = map.Compile("host", "reorder_test", "predict")
    result2 = compiled.Compute(x)
    testing.ProcessTest("Testing ReorderDataNode Compiled",
                        testing.IsEqual(np.array(result2), expected.ravel()))


def test_multiply(testing):

    # Test a model that multiplies input vector by a constant matrix
    model = ell.model.Model()

    x = np.array([1, 2, 3])
    layout = ell.model.PortMemoryLayout([int(3)])
    input = model.AddInput(layout, ell.nodes.PortType.real)

    a = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9]).astype(np.float).reshape((3, 3))
    matrix = model.AddConstant(a.ravel(), ell.model.PortMemoryLayout([3, 3]), ell.nodes.PortType.real)
    multiply = model.AddMatrixMultiply(matrix, input)
    output = model.AddOutput(layout, multiply)

    map = ell.model.Map(model, input, output)

    expected = a.dot(x)
    result = map.Compute(x)

    testing.ProcessTest("Testing AddMatrixMultiply Compute",
                        testing.IsEqual(np.array(result), expected))

    compiled = map.Compile("host", "multiply_test", "predict")

    result2 = compiled.Compute(x)

    testing.ProcessTest("Testing AddMatrixMultiply Compiled",
                        testing.IsEqual(np.array(result2), expected))


class UnaryTest:
    def __init__(self, name, op, func):
        self.name = name
        self.op = op
        self.func = func


def hard_sigmoid(x):
    scale = 0.2
    bias = 0.5
    if x < -bias / scale:
        return 0
    if x > (1 - bias) / scale:
        return 1
    return (scale * x) + bias


def sigmoid(x):
    return 1.0 / (math.exp(-x) + 1)


def softmax(x):
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)


def sign(x):
    if x >= 0:
        return 1
    return -1


def test_unary(testing):

    # Test a model that performs a unary operation
    for test in [UnaryTest("abs", ell.nodes.UnaryOperationType.abs, lambda x: abs(x)),
                 UnaryTest("cos", ell.nodes.UnaryOperationType.cos, lambda x: math.cos(x)),
                 UnaryTest("exp", ell.nodes.UnaryOperationType.exp, lambda x: math.exp(x)),
                 UnaryTest("hardSigmoid", ell.nodes.UnaryOperationType.hardSigmoid, lambda x: hard_sigmoid(x)),
                 UnaryTest("log", ell.nodes.UnaryOperationType.log, lambda x: math.log(x)),
                 UnaryTest("sigmoid", ell.nodes.UnaryOperationType.sigmoid, lambda x: sigmoid(x)),
                 UnaryTest("sign", ell.nodes.UnaryOperationType.sign, lambda x: sign(x)),
                 UnaryTest("sin", ell.nodes.UnaryOperationType.sin, lambda x: math.sin(x)),
                 UnaryTest("softmax", ell.nodes.UnaryOperationType.softmax, lambda x: softmax(x)),
                 UnaryTest("sqrt", ell.nodes.UnaryOperationType.sqrt, lambda x: math.sqrt(x)),
                 UnaryTest("square", ell.nodes.UnaryOperationType.square, lambda x: x * x),
                 UnaryTest("tanh", ell.nodes.UnaryOperationType.tanh, lambda x: math.tanh(x))]:

        model = ell.model.Model()

        layout = ell.model.PortMemoryLayout([int(10)])
        input = model.AddInput(layout, ell.nodes.PortType.smallReal)

        a = np.array(range(10)).astype(np.float32) + 1
        multiply = model.AddUnaryOperation(input, test.op)
        output = model.AddOutput(layout, multiply)

        expected = np.array([test.func(x) for x in a])
        if test.name == "softmax":
            expected = softmax(a)

        map = ell.model.Map(model, input, output)

        result = map.Compute(a)

        testing.ProcessTest("Testing AddUnaryOperation {}".format(test.name),
                            testing.IsEqual(np.array(result), expected, tol=1e-6))

        compiled = map.Compile("host", "test", "predict")

        result = compiled.Compute(a)

        testing.ProcessTest("Testing AddUnaryOperation Compiled {}".format(test.name),
                            testing.IsEqual(np.array(result), expected, tol=1e-6))


def load_vad_data():
    vad_data = os.path.join(script_path, "..", "..", "..", "libraries", "dsp", "VadData.txt")
    if not os.path.isfile(vad_data):
        vad_data = os.path.join(script_path, "..", "..", "..", "libraries", "dsp", "test", "src", "VadData.txt")

    dataset = ell.data.AutoSupervisedDataset()
    dataset.Load(vad_data)
    return dataset


def test_voice_activity_node(testing):
    sample_rate = 8000  # this is the same rate used to generate VadData.txt
    frame_duration = 0.032
    tau_up = 1.54
    tau_down = 0.074326
    large_input = 2.400160
    gain_att = 0.002885
    threshold_up = 3.552713
    threshold_down = 0.931252
    level_threshold = 0.007885

    builder = ell.model.ModelBuilder()
    ell_model = ell.model.Model()

    dataset = load_vad_data()
    size = dataset.NumFeatures()

    input_shape = ell.model.PortMemoryLayout([int(size)])
    output_shape = ell.model.PortMemoryLayout([1])

    input_node = builder.AddInputNode(ell_model, input_shape, ell.nodes.PortType.real)
    vad_node = builder.AddVoiceActivityDetectorNode(
        ell_model, ell.nodes.PortElements(input_node.GetOutputPort("output")), sample_rate, frame_duration,
        tau_up, tau_down, large_input, gain_att, threshold_up, threshold_down, level_threshold)
    # cast the integer output of VAD to double since our CompiledMap doesn't yet support having
    # different input and output types.
    cast_node = builder.AddTypeCastNode(ell_model, ell.nodes.PortElements(vad_node.GetOutputPort("output")),
                                        ell.nodes.PortType.real)
    output_node = builder.AddOutputNode(ell_model, output_shape,
                                        ell.nodes.PortElements(cast_node.GetOutputPort("output")))

    map = ell.model.Map(ell_model, input_node, ell.nodes.PortElements(output_node.GetOutputPort("output")))
    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False  # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.Compile("host", "vadtest", "predict", compiler_settings, optimizer_options)

    line = 1
    errors = 0
    for i in range(dataset.NumExamples()):
        row = dataset.GetExample(i)
        expected = row.GetLabel()
        data = row.GetData().ToArray()
        # watch out for AutoDataVector compression
        if len(data) < size:
            data.resize(size)

        value = compiled_map.Compute(data)[0]
        if value != expected:
            print("### error on line {}, signam={}, expected={}".format(line, value, expected))
            errors += 1
        line += 1

    testing.ProcessTest("test_voice_activity_node, errors={}".format(errors), errors == 0)


def create_tensor(value, size, rows, columns, channels):
    a = np.ones(size) * value
    a = a.reshape(rows, columns, channels)
    return ell.math.DoubleTensor(a)


def test_gru_node_with_vad_reset(testing):
    sample_rate = 8000  # this is the same rate used to generate VadData.txt
    frame_duration = 0.032
    tau_up = 1.54
    tau_down = 0.074326
    large_input = 2.400160
    gain_att = 0.002885
    threshold_up = 3.552713
    threshold_down = 0.931252
    level_threshold = 0.007885

    hidden_units = 10
    errors = 0

    builder = ell.model.ModelBuilder()
    ell_model = ell.model.Model()

    dataset = load_vad_data()
    input_size = dataset.NumFeatures()

    input_shape = ell.model.PortMemoryLayout([int(input_size)])
    output_shape = ell.model.PortMemoryLayout([hidden_units])
    dataType = ell.nodes.PortType.smallReal

    input_node = builder.AddInputNode(ell_model, input_shape, dataType)
    vad_node = builder.AddVoiceActivityDetectorNode(
        ell_model,
        ell.nodes.PortElements(input_node.GetOutputPort("output")), sample_rate, frame_duration,
        tau_up, tau_down, large_input, gain_att, threshold_up, threshold_down, level_threshold)

    numRows = hidden_units * 3
    numCols = input_size
    input_weights = np.ones(numRows * numCols) * 0.01
    numCols = hidden_units
    hidden_weights = np.ones(numRows * numCols) * 0.02
    input_bias = np.ones(numRows) * 0.01
    hidden_bias = np.ones(numRows) * 0.02

    input_weights_node = builder.AddConstantNode(ell_model, input_weights, ell.nodes.PortType.smallReal)
    hidden_weights_node = builder.AddConstantNode(ell_model, hidden_weights, ell.nodes.PortType.smallReal)
    input_bias_node = builder.AddConstantNode(ell_model, input_bias, ell.nodes.PortType.smallReal)
    hidden_bias_node = builder.AddConstantNode(ell_model, hidden_bias, ell.nodes.PortType.smallReal)

    # now create a gru_node that takes the same input as the vad_node, and also takes
    # the output of the vad_node as a reset signal.
    gru_node = builder.AddGRUNode(
        ell_model,
        ell.nodes.PortElements(input_node.GetOutputPort("output")),
        ell.nodes.PortElements(vad_node.GetOutputPort("output")),
        hidden_units,
        ell.nodes.PortElements(input_weights_node.GetOutputPort("output")),
        ell.nodes.PortElements(hidden_weights_node.GetOutputPort("output")),
        ell.nodes.PortElements(input_bias_node.GetOutputPort("output")),
        ell.nodes.PortElements(hidden_bias_node.GetOutputPort("output")),
        ell.neural.ActivationType.tanh,
        ell.neural.ActivationType.sigmoid)

    output_node = builder.AddOutputNode(
        ell_model, output_shape,
        ell.nodes.PortElements(gru_node.GetOutputPort("output")))

    # test we can access GetMemoryLayout information on the ports.
    output_size = list(gru_node.GetOutputPort("output").GetMemoryLayout().size)
    expected_size = [hidden_units]
    if output_size != expected_size:
        print("The output port on the gru_node has size {}, we are expecting {}".format(output_size, expected_size))
        errors += 1

    map = ell.model.Map(ell_model, input_node, ell.nodes.PortElements(output_node.GetOutputPort("output")))
    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False  # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.Compile("host", "gruvadtest", "predict", compiler_settings, optimizer_options)

    last_signal = 0
    was_reset = False
    for i in range(dataset.NumExamples()):
        row = dataset.GetExample(i)
        expected = row.GetLabel()
        data = row.GetData().ToArray()
        # watch out for AutoDataVector compression
        if len(data) < input_size:
            data.resize(input_size)
        value = np.array(compiled_map.Compute(data))
        total = np.sum(value)

        if was_reset and total > 0.1:
            errors += 1

        if last_signal == 1 and expected == 0:
            # reset should have happened which means the next sum must be close to zero.
            was_reset = True
        else:
            was_reset = False

    testing.ProcessTest("test_gru_node_with_vad_reset, errors={}".format(errors), errors == 0)


def hamming_callback(testing, map, iteration):
    size = map.GetInputShape().Size()
    expected = np.hamming(size)
    input = np.ones(size)
    output = map.Compute(input)
    testing.ProcessTest("test_hamming_node compute iteration {}".format(iteration), np.allclose(output, expected))

    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False  # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.Compile("host", "hammingtest", "predict", compiler_settings, optimizer_options)

    compiled_output = compiled_map.Compute(input)
    testing.ProcessTest("test_hamming_node compiled iteration {}".format(iteration),
                        np.allclose(compiled_output, expected))


def test_hamming_node(testing):
    mb = ell.model.ModelBuilder()
    model = ell.model.Model()

    size = 400

    input_shape = ell.model.PortMemoryLayout([size])
    output_shape = ell.model.PortMemoryLayout([size])

    input_node = mb.AddInputNode(model, input_shape, ell.nodes.PortType.real)
    hamming_node = mb.AddHammingWindowNode(model, ell.nodes.PortElements(input_node.GetOutputPort("output")))
    outputNode = mb.AddOutputNode(model, output_shape, ell.nodes.PortElements(hamming_node.GetOutputPort("output")))

    map = ell.model.Map(model, input_node, ell.nodes.PortElements(outputNode.GetOutputPort("output")))

    test_with_serialization(testing, map, "test_hamming_node", hamming_callback)


def mel_filterbank_callback(testing, map, iteration):
    size = 512
    window_size = 512
    num_filters = 13
    sample_rate = 16000

    try:
        from python_speech_features import get_filterbanks
    except:
        print("### skiping test_mel_filterbank because 'python_speech_features' module is not available")
        return

    fbanks = get_filterbanks(num_filters, window_size, sample_rate)
    input = np.array(range(size)).astype(np.float)

    chopped = input[0:fbanks.shape[1]]
    expected = np.dot(chopped, fbanks.T)

    output = map.Compute(input)
    testing.ProcessTest("test_mel_filterbank compute iteration {}".format(iteration), np.allclose(output, expected))

    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False  # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.Compile("host", "hammingtest", "predict", compiler_settings, optimizer_options)

    compiled_output = compiled_map.Compute(input)
    testing.ProcessTest("test_mel_filterbank compiled iteration {}".format(iteration),
                        np.allclose(compiled_output, expected))


def test_mel_filterbank(testing):

    mb = ell.model.ModelBuilder()
    model = ell.model.Model()

    size = 512
    num_filters = 13
    sample_rate = 16000

    input_shape = ell.model.PortMemoryLayout([size])
    output_shape = ell.model.PortMemoryLayout([num_filters])

    input_node = mb.AddInputNode(model, input_shape, ell.nodes.PortType.real)
    filterbank_node = mb.AddMelFilterBankNode(model, ell.nodes.PortElements(input_node.GetOutputPort("output")),
                                              sample_rate, size, num_filters, num_filters)
    outputNode = mb.AddOutputNode(model, output_shape, ell.nodes.PortElements(filterbank_node.GetOutputPort("output")))

    map = ell.model.Map(model, input_node, ell.nodes.PortElements(outputNode.GetOutputPort("output")))

    test_with_serialization(testing, map, "test_mel_filterbank", mel_filterbank_callback)


def fftnode_callback(testing, map, iteration):
    inputSize = int(map.GetMetadataValue("inputSize"))
    fftSize = int(map.GetMetadataValue("fftSize"))
    if fftSize == 0:
        fftSize = int(math.pow(2, math.ceil(math.log2(inputSize))))

    a = np.array([float(i) * math.pi / 180 for i in range(inputSize)])
    y1 = np.sin(a * 10)
    y2 = np.sin(a * 20)
    y3 = np.sin(a * 50)
    signal = y1 + y2 + y3

    expected = np.absolute(np.fft.rfft(signal, n=fftSize))

    output = map.Compute(signal)
    expected = expected[0:len(output)]  # ell returns size/2, numpy returns (size/2)+1
    filename = "ffttest_{}_{}_{}.npz".format(inputSize, fftSize, iteration)
    np.savez(filename, output=np.array(output), expected=np.array(expected))
    testing.ProcessTest("test_fftnode compute iteration {}".format(iteration), np.allclose(output, expected))

    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False  # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.Compile("host", "ffttest", "predict", compiler_settings, optimizer_options)

    compiled_output = compiled_map.Compute(signal)

    testing.ProcessTest("test_fftnode compiled iteration {}".format(iteration), np.allclose(compiled_output, output))


def test_fftnode_size(testing, inputSize, fftSize):
    mb = ell.model.ModelBuilder()
    model = ell.model.Model()

    input_shape = ell.model.PortMemoryLayout([inputSize])

    input_node = mb.AddInputNode(model, input_shape, ell.nodes.PortType.real)
    if (fftSize == 0):
        fft_node = mb.AddFFTNode(model, ell.nodes.PortElements(input_node.GetOutputPort("output")))
    else:
        fft_node = mb.AddFFTNode(model, ell.nodes.PortElements(input_node.GetOutputPort("output")), fftSize)
    output_size = fft_node.GetOutputPort("output").Size()
    output_shape = ell.model.PortMemoryLayout([output_size])
    outputNode = mb.AddOutputNode(model, output_shape, ell.nodes.PortElements(fft_node.GetOutputPort("output")))

    map = ell.model.Map(model, input_node, ell.nodes.PortElements(outputNode.GetOutputPort("output")))
    map.SetMetadataValue("inputSize", str(inputSize))
    map.SetMetadataValue("fftSize", str(fftSize))

    test_with_serialization(testing, map, "test_fftnode ({})".format(fftSize), fftnode_callback)


class TypeCastInfo:
    def __init__(self, name, t):
        self.name = name
        self.t = t

    def create_vector(self, len):
        if self.t == ell.nodes.PortType.real:
            return ell.math.DoubleVector(len)
        elif self.t == ell.nodes.PortType.smallReal:
            return ell.math.FloatVector(len)
        elif self.t == ell.nodes.PortType.integer:
            return ell.math.IntVector(len)
        elif self.t == ell.nodes.PortType.bigInt:
            return ell.math.Int64Vector(len)
        raise Exception("Unsupported type")

    def cast_vector(self, a):
        if self.t == ell.nodes.PortType.real:
            return a.astype(np.float)
        elif self.t == ell.nodes.PortType.smallReal:
            return a.astype(np.float32)
        elif self.t == ell.nodes.PortType.integer:
            return a.astype(np.int)
        elif self.t == ell.nodes.PortType.bigInt:
            return a.astype(np.int64)
        raise Exception("Unsupported type")


def test_typecast(testing):
    # Test a model that has differen types callbacks.
    for t in [TypeCastInfo("real", ell.nodes.PortType.real),
              TypeCastInfo("smallReal", ell.nodes.PortType.smallReal),
              TypeCastInfo("integer", ell.nodes.PortType.integer),
              # TypeCastInfo("bigInt", ell.nodes.PortType.bigInt)
              ]:

        model = ell.model.Model()

        # input to a SourceNode is a timestamp.
        layout = ell.model.PortMemoryLayout([int(10)])
        input = model.AddInput(layout, ell.nodes.PortType.real)
        cast = model.AddTypeCast(input, t.t)

        output = model.AddOutput(layout, cast)
        map = ell.model.Map(model, input, output)

        a = np.array(range(10)).astype(np.float) / 2
        expected = np.array(a)

        i = ell.math.DoubleVector(a)
        o = t.create_vector(len(a))
        expected = t.cast_vector(a)
        map.ComputeMultiple([i], [o])

        testing.ProcessTest("Testing TypeCast to type {} Compute".format(t.name),
                            testing.IsEqual(np.array(o), expected))

        compiled = map.Compile("host", "test", "predict")
        o = t.create_vector(len(a))
        compiled.ComputeMultiple([i], [o])
        testing.ProcessTest("Testing TypeCast to type {} Compiled".format(t.name),
                            testing.IsEqual(np.array(o), expected))


def test_fftnode(testing):
    try:
        test_fftnode_size(testing, 100, 100)
        testing.ProcessTest("test_fftnode needs updating for sizes that are not a power of 2.", False)
    except Exception as e:
        testing.ProcessTest("test_fftnode: {}".format(e), True)

    test_fftnode_size(testing, 64, 64)
    test_fftnode_size(testing, 100, 128)
    test_fftnode_size(testing, 20, 16)
    test_fftnode_size(testing, 20, 0)


def test():
    testing = Testing()
    test_reorder(testing)
    test_typecast(testing)
    test_unary(testing)
    # test_multiply(testing)  # bugbug: crashing on Linux...
    test_voice_activity_node(testing)
    test_gru_node_with_vad_reset(testing)
    test_hamming_node(testing)
    test_mel_filterbank(testing)
    test_fftnode(testing)
    return 0


if __name__ == "__main__":
    test()
