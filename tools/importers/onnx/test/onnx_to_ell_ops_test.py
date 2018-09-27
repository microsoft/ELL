import os
import sys
import logging
import time
import unittest

import numpy as np 
import torch
import torch.nn as nn
import torch.nn.functional as F
import onnx

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 'utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import find_ell
import ell
import common.memory_shapes as memory_shapes
from onnx_to_ell import convert_onnx_to_ell as onnx_converter


_logger = logging.getLogger(__name__)

# handmade custom model
# tensor = np.randon.randn()
# custom_model = []
# input_node = { 
#     'name'           : '0', 
#     'op_type'        : 'Input',
#     'input'          : [],
#     'output'         : ['0'],
#     'attributes'     : {},
#     'input_shape'    : [],
#     'output_shape'   : [((3, 32, 32), 'channel_row_column')],
#     'padding'        : {'size': 0, 'scheme': 0},
#     'output_padding' : {'size': 0, 'scheme': 0},
#     'weights'        : {}
#     }

# conv1 = { 
#     'name'           : '12', 
#     'op_type'        : 'Convolution',
#     'input'          : ['0'],
#     'output'         : ['12'],
#     'attributes'     : {'size': 5, 'stride': 1, 'padding': 0, 'dilations': 1},
#     'input_shape'    : [((3, 32, 32), 'channel_row_column')],
#     'output_shape'   : [((10, 28, 28), 'channel_row_column')],
#     'padding'        : {'size': 0, 'scheme': 0},
#     'output_padding' : {'size': 0, 'scheme': 0},
#     'weights'        : {'weights': ('1', weight_tensor, 'channel_row_column'),
#                         'bias': ('2', bias_tensor, 'channel')}
#     }

# maxpool2d_1 = { 
#     'name'           : '13', 
#     'op_type'        : 'MaxPooling',
#     'input'          : ['12'],
#     'output'         : ['13'],
#     'attributes'     : {'size': 5, 'stride': 5, 'padding': 2},
#     'input_shape'    : [((10, 28, 28), 'channel_row_column')],
#     'output_shape'   : [((10, 14, 14), 'channel_row_column')],
#     'padding'        : {'size': 0, 'scheme': 0},
#     'output_padding' : {'size': 0, 'scheme': 0},
#     'weights'        : {'weights': ('1', tensor, 'channel_row_column'),
#                         'bias': ('2', tensor, 'channel')}
#     }

logging.basicConfig(level=logging.DEBUG, format="%(message)s")

class _ConvNd(nn.Module):

    def __init__(self, in_channels, out_channels, kernel_size, weight, stride,
                 padding, dilation, transposed, output_padding, groups,  bias):
        super(_ConvNd, self).__init__()
        if in_channels % groups != 0:
            raise ValueError('in_channels must be divisible by groups')
        if out_channels % groups != 0:
            raise ValueError('out_channels must be divisible by groups')
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.kernel_size = kernel_size
        self.stride = stride
        self.padding = padding
        self.dilation = dilation
        self.transposed = transposed
        self.output_padding = output_padding
        self.groups = groups
        if transposed:
            self.weight = nn.Parameter(torch.Tensor(
                in_channels, out_channels // groups, *kernel_size))
        else:
            self.weight = nn.Parameter(torch.Tensor(
                out_channels, in_channels // groups, *kernel_size))
        if bias:
            self.bias = nn.Parameter(torch.Tensor(out_channels))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

    def reset_parameters(self):
        n = self.in_channels
        for k in self.kernel_size:
            n *= k
        stdv = 1. / math.sqrt(n)
        self.weight.data = weight
        if self.bias is not None:
            self.bias.data = bias

    def __repr__(self):
        s = ('{name}({in_channels}, {out_channels}, kernel_size={kernel_size}'
             ', stride={stride}')
        if self.padding != (0,) * len(self.padding):
            s += ', padding={padding}'
        if self.dilation != (1,) * len(self.dilation):
            s += ', dilation={dilation}'
        if self.output_padding != (0,) * len(self.output_padding):
            s += ', output_padding={output_padding}'
        if self.groups != 1:
            s += ', groups={groups}'
        if self.bias is None:
            s += ', bias=False'
        s += ')'
        return s.format(name=self.__class__.__name__, **self.__dict__)

class Conv2d_custom(_ConvNd):
    def __init__(self, in_channels, out_channels, kernel_size, weight, stride=1,
                 padding=0, dilation=1, groups=1, bias=True):
        kernel_size = kernel_size
        stride = stride
        padding = padding
        dilation = dilation
        super(Conv2d_custom, self).__init__(
            in_channels, out_channels, kernel_size, weight, stride, padding, dilation,
            False, 0, groups, bias)

    def forward(self, input):
        return F.conv2d(input, self.weight, self.bias, self.stride,
                        self.padding, self.dilation, self.groups)

class FC_custom(nn.Module):
    def __init__(self, input_features, output_features, weight, bias=None):
        super(FC_custom, self).__init__()
        self.input_features = input_features
        self.output_features = output_features
 
        # nn.Parameter is a special kind of Tensor, that will get
        # automatically registered as Module's parameter once it's assigned
        # as an attribute. Parameters and buffers need to be registered, or
        # they won't appear in .parameters() (doesn't apply to buffers), and
        # won't be converted when e.g. .cuda() is called. You can use
        # .register_buffer() to register buffers.
        # nn.Parameters require gradients by default.

        self.weight = nn.Parameter(torch.Tensor(output_features, input_features))
        if bias is not None:
            self.bias = nn.Parameter(torch.Tensor(output_features))
        else:
            self.register_parameter('bias', None)

        # initialize weights and bias
        if isinstance(weight, torch.Tensor) == False:
            weight = torch.from_numpy(weight)
        self.weight.data = weight
        if bias is not None:
            if isinstance(bias, torch.Tensor) == False:
                bias = torch.from_numpy(bias)
            self.bias.data = bias

    def linear(self, input, weight, bias=None):
        output = input.mm(weight.t())
        if bias is not None:
            output += bias.unsqueeze(0).expand_as(output)
        return output

    def forward(self, input):
        return self.linear(input, self.weight, self.bias)


def fc_manual_test(node, test_tensor, in_channels, out_channels, fc_w, fc_b):
    flatten_shape = int(test_tensor.size(1))*int(test_tensor.size(2))*int(test_tensor.size(3))
    flattened_input = test_tensor.view(-1, flatten_shape)
    _logger.info("Weights: ", fc_w)
    _logger.info("Test input flattened: ", flattened_input)

    custom_fc = FC_custom(in_channels, out_channels, fc_w, bias=fc_b)
    custom_fc.eval()
    with torch.no_grad():
        matmul_result = custom_fc.forward(flattened_input)
    return matmul_result

def conv2d_manual_test(node, test_tensor, in_channels, out_channels, k, w_, s, p, d, b_):
    conv_op = Conv2d_custom(in_channels, out_channels, k, w_, s, p, d, bias=b_)
    conv_op.eval()
    with torch.no_grad():
        output = conv_op.forward(test_tensor)
    return output

def test_graph(onnx_nodes, test_tensor):
    w_ = None
    b_ = None
    k = None # kernel size
    p = None # padding
    s = None # strides
    d = None # dilation
    in_channels = None
    out_channels = None
    
    for node in onnx_nodes:
        if node.op_type == "Input":
            continue
        if 'size' in node.attribute:
            k = node.attribute['size']
            s = node.attribute['stride']
        if 'padding' in node.attribute:
            p = node.attribute['padding']
        if 'dilation' in node.attribute:
            d = node.attribute['dilation']
        in_channels = node.input_shape[0][0][0]
        out_channels = node.output_shape[0][0][0]
        if node.weights is not None:
            for w in node.weights:
                _logger.info("Weight Type", w)
                weight = node.weights[w]
                if w == 'weights':
                    w_ = weight[1]
                elif w == 'bias':
                    b_ = weight[1]

        if node.op_type == "Convolution":
            return conv2d_manual_test(node, test_tensor, in_channels, out_channels, k, w_, s, p, d, b_)
        if node.op_type == "FullyConnected":
            return fc_manual_test(node, test_tensor, in_channels, out_channels, w_, b_)

def export_to_onnx(model, input_shape=(1,3,224,224)):
    tensor = torch.randn(input_shape)

    model.forward(tensor)
    _logger.info("Exporting model to ONNX")
    torch_out = torch.onnx._export(model, tensor, "model.onnx", export_params=True, verbose=True)


def verify_ell_model(onnx_model):
    """
    Test each operation in the onnx graph by creating a 
    custom pytorch layer for each node then run forward 
    with the onnx node weight on both ell and pytorch node
    """

    _logger.info("Model verification started")
    try:

        ell_map, onnx_nodes = onnx_converter(onnx_model)
        ell_map.Save("model.ell")

        # get model input shape
        ell_input_shape = ell_map.GetInputShape()
        ell_shape = (ell_input_shape.rows, ell_input_shape.columns, ell_input_shape.channels)
        model_input_shape =  (ell_shape[2], ell_shape[0], ell_shape[1])
        test_input = torch.randn((1, model_input_shape[0], model_input_shape[1], model_input_shape[2])) * 255

        order = "channel"
        if len(test_input.shape) == 4:
            order = "filter_channel_row_column"
        elif len(test_input.shape) == 3:
            order = "channel_row_column"
        elif len(test_input.shape) == 2:
            order = "row_column"

        ell_input_tensor = memory_shapes.get_tensor_in_ell_order(test_input.numpy(), order).ravel().astype(np.float32)
        ell_out = np.array(ell_map.Compute(ell_input_tensor, dtype=np.float32))
        _logger.info("############ ell_output:", ell_out)

        model.eval()

        torch_out = model.forward(test_input)
        _logger.info("############ torch_output:", torch_out)

        ell_out = ell_out.reshape(torch_out.size())
        # np.testing.assert_almost_equal(torch_out.data.cpu().numpy(), ell_out, decimal=3, err_msg=(
        #                 'results for compiled ELL model do not match ONNX output!'))

        # Manual test
        _logger.info("Manual test: \n")
        manual_compute_result = test_graph(onnx_nodes, test_input)
        np.testing.assert_array_almost_equal(
                    manual_compute_result.detach().numpy(), ell_out, decimal=4, err_msg=(
                        'results for compute ELL model do not match ONNX output!'))
        _logger.info("Verified compute result against ONNX")
        _logger.info("########### Compute test passed ########### ")
        _logger.info("ell_output:", ell_out)
        _logger.info("torch_output", torch_out)
        _logger.info("onnx_output", manual_compute_result)
        _logger.info("###################### ")

        # Get compiled ELL result
        _logger.info("Getting compiled ELL results")
        compiler_options = ell.model.MapCompilerOptions()
        compiler_options.useBlas = True
        compiled_ell_map = ell_map.Compile("host", "model", "predict", compilerOptions=compiler_options, dtype=np.float32)

        result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))
        ell_out_compiled = result_from_compiled.reshape(torch_out.size())

        # Verify the computed result against the onnx result
        np.testing.assert_array_almost_equal(
            torch_out, ell_out_compiled, decimal=4, err_msg=(
                'results for compiled ELL model do not match ONNX output!'))
        _logger.info("Verified compiled result against ONNX")
    except BaseException as exception:
        _logger.error("Verification of model output failed")
        raise exception

    _logger.info("Verification of model output complete")

# A torch model for testing
class Net(nn.Module): 
        def __init__(self):
            super(Net, self).__init__()
            self.fc1 = nn.Linear( 3*3*3, 1, bias=False)

            fc_weight = torch.randn(1, 27)
            self.fc1.weight.data = fc_weight
            _logger.info(fc_weight)


        def forward(self, x):
            x1 = x.view(-1, 3*3*3)
            x1 = self.fc1(x1)
            return x1 


class OnnxTestCase(unittest.TestCase):
    def setUp(self):
        self.onnx_model = 'model.onnx'
        self.torch_model = 'model.pth'

    def generate_model_test(self):
        model = Net()
        export_to_onnx(model, (1,3,3,3))
        verify_ell_model("model.onnx")

    def external_model_test(self):
        """
        Loading tytorch model depend on how it's been saved. 
        torch.save/torch.load "saves/loads" an object to a disk file.

        So, if you save the_model, it will save the entire model object, 
        including its architecture definition and some other internal aspects.
        If you save the_model.state_dict(), it will save a dictionary containing 
        the model state (i.e. parameters and buffers) only. 
        Saving the model can break the code in various ways, so the preferred 
        method is to save and load only the model state
        e.g : if we use torch.save("model.pth") it's load as torch_model = torch.load('model.pht')
        however if we save the state_dict(), we load the model as
        "torch_model_obj.load_state_dict(torch.load(torch_model))" notice you'll need the model definition here
        """

        # if torch_model_obj is not None:
        #     torch_model = torch_model_obj.load_state_dict(torch.load(torch_model))
        
        torch_model = torch.load(self.onnx_model)
        ell_map, onnx_nodes = onnx_converter(self.onnx_model)
        verify_ell_model(onnx_nodes)


if __name__ == "__main__":
    unittest.main()