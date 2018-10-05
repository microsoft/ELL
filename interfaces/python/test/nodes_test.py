import os
import sys
from testing import Testing
import numpy as np
import ell_helper
import ell

script_path = os.path.dirname(os.path.abspath(__file__))

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
    
    input_shape = ell.math.TensorShape(1, 1, size)
    output_shape = ell.math.TensorShape(1, 1, 1)

    input_node = builder.AddInputNode(ell_model, input_shape, ell.nodes.PortType.real)
    vad_node = builder.AddVoiceActivityDetectorNode(ell_model, ell.nodes.PortElements(input_node.GetOutputPort("output")), sample_rate, frame_duration,
        tau_up, tau_down, large_input, gain_att, threshold_up, threshold_down, level_threshold)
    # cast the integer output of VAD to double since our CompiledMap doesn't yet support having different input and output types.
    cast_node = builder.AddTypeCastNode(ell_model, ell.nodes.PortElements(vad_node.GetOutputPort("output")), ell.nodes.PortType.real)
    output_node = builder.AddOutputNode(ell_model, output_shape, ell.nodes.PortElements(cast_node.GetOutputPort("output")))

    map = ell.model.Map(ell_model, input_node, ell.nodes.PortElements(output_node.GetOutputPort("output")))
    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.CompileFloat("host", "vadtest", "predict", compiler_settings, optimizer_options)

    line = 1
    errors = 0
    for i in range(dataset.NumExamples()):
        row = dataset.GetExample(i)
        expected = row.GetLabel()
        data = row.GetData().ToArray()
        # watch out for AutoDataVector compression
        if len(data) < size:
            data.resize(size)            
        
        value = compiled_map.Compute(data, dtype=np.float)[0]
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
    
    input_shape = ell.math.TensorShape(1, 1, input_size)
    output_shape = ell.math.TensorShape(1, 1, hidden_units)
    dataType = ell.nodes.PortType.smallReal

    input_node = builder.AddInputNode(ell_model, input_shape, dataType)
    vad_node = builder.AddVoiceActivityDetectorNode(ell_model, 
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
    gru_node = builder.AddGRUNode(ell_model, 
        ell.nodes.PortElements(input_node.GetOutputPort("output")), 
        ell.nodes.PortElements(vad_node.GetOutputPort("output")), 
        hidden_units,
        ell.nodes.PortElements(input_weights_node.GetOutputPort("output")), 
        ell.nodes.PortElements(hidden_weights_node.GetOutputPort("output")), 
        ell.nodes.PortElements(input_bias_node.GetOutputPort("output")), 
        ell.nodes.PortElements(hidden_bias_node.GetOutputPort("output")), 
        ell.neural.ActivationType.tanh, 
        ell.neural.ActivationType.sigmoid)
    
    output_node = builder.AddOutputNode(ell_model, output_shape, 
        ell.nodes.PortElements(gru_node.GetOutputPort("output")))

    # test we can access GetMemoryLayout information on the ports.
    output_size = list(gru_node.GetOutputPort("output").GetMemoryLayout().size)
    expected_size = [hidden_units]
    if output_size != expected_size:
        print("The output port on the gru_node has size {}, we are expecting {}".format(output_size, expected_size))
        errors += 1

    map = ell.model.Map(ell_model, input_node, ell.nodes.PortElements(output_node.GetOutputPort("output")))
    compiler_settings = ell.model.MapCompilerOptions()
    compiler_settings.useBlas = False # not resolvable on our Linux test machines...
    optimizer_options = ell.model.ModelOptimizerOptions()
    compiled_map = map.CompileFloat("host", "gruvadtest", "predict", compiler_settings, optimizer_options)

    line = 1
    last_signal = 0
    was_reset = False
    for i in range(dataset.NumExamples()):
        row = dataset.GetExample(i)
        expected = row.GetLabel()
        data = row.GetData().ToArray()        
        # watch out for AutoDataVector compression
        if len(data) < input_size:
            data.resize(input_size)            
        value = compiled_map.Compute(list(data), dtype=np.float32)
        total = np.sum(value)

        if was_reset and total > 0.1:
            errors += 1
            
        if last_signal == 1 and expected == 0:
            # reset should have happened which means the next sum must be close to zero.
            was_reset = True
        else:
            was_reset = False
    
    testing.ProcessTest("test_gru_node_with_vad_reset, errors={}".format(errors), errors == 0)


def test():    
    testing = Testing()
    test_voice_activity_node(testing)
    test_gru_node_with_vad_reset(testing)
    return 0

if __name__ == "__main__":
    test()
