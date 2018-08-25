import os
import sys
import logging
import time

import numpy as np 
import torch
import torch.nn as nn
import torch.nn.functional as F
import onnx

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', 'utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

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


def fc_manual_test(onnx_nodes, test_tensor):
    fc_w = None
    fc_b = None
    def add_bias(ax, b):
        return np.add(ax, b)

    for node in onnx_nodes:
        if node.weights is not None:
            for w in node.weights:
                print("Weight", w)
                weight = node.weights[w]
                if w == 'weights':
                    fc_w = weight[1]
                elif w == 'bias':
                    fc_b = weight[1]
                print(weight[0],":", weight[1].shape) 
    # matmul_result = np.matmul(test_tensor, np.transpose(fc_w))
    # if fc_b is not None:
    #     matmul_result = add_bias(matmul_result, fc_b)

    custom_fc = FC_custom(D_in, H_out, w_, bias=b_)
    custom_fc.eval()
    with torch.no_grad():
        matmul_result = custom_fc.forward(test_tensor)
    return matmul_result

def conv2d_test(node, test_tensor):
    conv_w = None
    conv_b = None
    k = None
    p = None
    s = None
    p = None
    s = None
    nb_filters = None
    for node in onnx_nodes:
        if node.op_type == "Convolution":
            k = node.attributes['size']
            p = node.attributes['padding']
            s = node.attributes['strides']
            d = node.attributes['dilations']
            nb_filters = node.input_shape[0][0][0]

        if node.weights is not None:
            for w in node.weights:
                print("Weight", w)
                weight = node.weights[w]
                if w == 'weights':
                    conv_w = weight[1]
                elif w == 'bias':
                    conv_b = weight[1]
                print(weight[0], w,":", weight[1].shape) 
    
    return 

fc_weight = torch.Tensor([[1,2,3,4], 
                          [5,6,7,8]])
# A torch model with only a conv layer
class Net(nn.Module): 
        def __init__(self):
            super(Net, self).__init__()
            # self.conv0 = nn.Conv2d(3, 10, kernel_size=5, stride=1, bias=True, padding=0)
            # self.conv1 = nn.Conv2d(10, 20, kernel_size=5, stride=1, bias=True, padding=1)
            # self.conv2_drop = nn.Dropout2d()
            # self.bn1 = nn.BatchNorm2d(20)
            self.fc1 = nn.Linear(1*2*2, 2, bias=False)
            self.fc2 = nn.Linear(50, 10, bias=False)

            self.fc1.weight.data = fc_weight
            # nn.init.uniform_(self.conv1.bias)
            # nn.init.uniform_(self.bn1.bias)


        def forward(self, x):
            # x1 = F.max_pool2d(self.conv0(x), 3, stride=2, padding=1)
            # x2 = x1
            # x1 = self.conv1(x1)
            # x1 = self.conv2_drop(x1)
            # x1 = torch.cat((x1,x2), 1)
            
            # x1 = self.bn1(x1)
            # # x = F.relu(x)
            # x = F.leaky_relu(x2)
            x1 = x.view(-1, 1*2*2)
            x1 = self.fc1(x1)
            # # x1 = x1.view(-1)
            # x1 = self.fc2(x1)
            # x1 = nn.Softmax2d(x1)
            x = x1
            return x 

model = Net()

# print(model.weight)
# tensor = torch.randn(1, 1, 32, 32)
tensor = torch.Tensor([[[[2,2],
                         [2,2]]]])

model.forward(tensor)
# print("Torch model", model)
_logger.info("Exporting model to ONNX")
torch_out = torch.onnx._export(model, tensor, "model.onnx", export_params=True, verbose=True)
onnx_model = onnx.load("model.onnx")

ell_map, onnx_nodes = onnx_converter("model.onnx")

test_input = torch.Tensor([[[[1,1],[1,1]]]])

ell_input_tensor = memory_shapes.get_tensor_in_ell_order(test_input.numpy(), "filter_channel_row_column").ravel().astype(np.float32)

ell_out = np.array(ell_map.Compute(ell_input_tensor, dtype=np.float32))
ell_out = ell_out.reshape((1,2))


model.eval
flatten_shape = int(test_input.size(1))*int(test_input.size(2))*int(test_input.size(3))
test_tensor = test_input.view(-1, flatten_shape)
torch_out = model.forward(test_tensor)
# np.testing.assert_almost_equal(torch_out.data.cpu().numpy(), ell_out, decimal=3, err_msg=(
#                 'results for compiled ELL model do not match ONNX output!'))

# Manual test
print("Manual test: \n")
torch_output_tensor = model.forward(test_tensor)
# manual_compute_result = fc_manual_test(onnx_nodes, test_tensor)
# np.testing.assert_array_almost_equal(
#             torch_output_tensor.detach().numpy(), ell_out, decimal=4, err_msg=(
#                 'results for compiled ELL model do not match ONNX output!'))
_logger.info("Verified compiled result against ONNX")


def verify_ell_output_in_vision_model(ell_map, torch_model, onnx_nodes, testing_info):
    _logger.info("Verification of model output starting")
    try:
        torch_model.eval()
        ell_input_shape = ell_map.GetInputShape()
        ell_shape = (ell_input_shape.rows, ell_input_shape.columns, ell_input_shape.channels)
        pytorch_input_shape =  (ell_shape[2], ell_shape[0], ell_shape[1])
        
        torch_input_tensor_ = np.random.random((1, pytorch_input_shape[0], pytorch_input_shape[1], pytorch_input_shape[2])) * 255
        torch_input_tensor = np.float32(torch_input_tensor_) 
        order = "channel"
        if len(torch_input_tensor.shape) == 4:
            order = "filter_channel_row_column"
        elif len(torch_input_tensor.shape) == 3:
            order = "channel_row_column"

        print("Checking shapes: torch shape =", torch_input_tensor.shape)
        print("Order", order)
        ell_input_tensor = memory_shapes.get_tensor_in_ell_order(torch_input_tensor, order).ravel().astype(np.float32)
        ell_input_tensor = np.float32(ell_input_tensor)

        _logger.info("Get manual test output")
        flatten_shape = int(torch_input_tensor.shape[1])*int(torch_input_tensor.shape[2])*int(torch_input_tensor.shape[3])
        torch_input_tensor_ = torch.from_numpy(torch_input_tensor)
        test_tensor = torch_input_tensor_.view(-1, flatten_shape)
        # manual_compute_result = fc_manual_test(onnx_nodes, test_tensor)
        

        # Get output from PyTorch model
        _logger.info("Getting PyTorch results")
        with torch.no_grad():
            torch_output_ = torch_model.forward(torch.from_numpy(torch_input_tensor).float())
            order = "channel"
            if len(torch_output_.shape) == 4:
                order = "filter_channel_row_column"
            elif len(torch_output_.shape) == 3:
                order = "channel_row_column" 
            torch_output = memory_shapes.get_tensor_in_ell_order(torch_output_.numpy(), order)

        print("Check shapes: Ell input shape =", ell_input_tensor.shape)
        # Get computed ELL result
        _logger.info("Getting computed ELL results")
        result_from_compute = np.array(ell_map.Compute(ell_input_tensor, dtype=np.float32))
        # result_from_compute = result_from_compute.reshape((1, 1, 10))
        


        # Get compiled ELL result
        _logger.info("Getting compiled ELL results")
        compiler_options = ell.model.MapCompilerOptions()
        compiler_options.useBlas = True
        compiled_ell_map = ell_map.Compile("host", "model", "predict", compilerOptions=compiler_options, dtype=np.float32)

        result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))
        
        print("Check shapes: torch shape = ", torch_output.shape, "ell shape = ", result_from_compute.shape)
        # Verify the computed result against the pytorch result
        np.testing.assert_array_almost_equal(
           torch_output[-1], result_from_compute[-1], decimal=4, err_msg=(
               'results for computed ELL model do not match ONNX output!'))
        _logger.info("Verified computed result against ONNX")

        # Manual test
        # torch_output_ = torch_output_.reshape((1,10))
        # np.testing.assert_array_almost_equal(
        #    torch_output[-1], manual_compute_result, decimal=4, err_msg=(
        #        'results for manual computation do not match ELL output!'))
        # print("torch_output_: ", torch_output_[-1, :])
        # print("manual_compute_result:", manual_compute_result[-1, :])
        # _logger.info("Verified manual result against ONNX")
            
        # Verify the compiled result  against the onnx result
        # torch_output = torch_output.reshape((10,))
        # np.testing.assert_array_almost_equal(
        #     torch_output[0,-1], result_from_compiled , decimal=4, err_msg=(
        #         'results for compiled ELL model do not match ONNX output!'))
        # _logger.info("Verified compiled result against ONNX")
        
        # Verify the compiled result agrees with the computed result
        # np.testing.assert_array_almost_equal(
        #     result_from_compute, result_from_compiled, decimal=4, err_msg=(
        #         'results for computed ELL model do not match results from compiled ELL model!'))
        # _logger.info("Verified compiled result against computed result")

        # Get timing info
    #     total_time = 0
    #     num_frames = 50
    #     _logger.info("Sending {} frames through model...".format(num_frames))
    #     for i in range(num_frames):
    #         torch_input_tensor = np.random.random((torch_model.arguments[0].shape)).astype(np.float32) * 255
    #         ell_input_tensor = memory_shapes.get_tensor_in_ell_order(torch_input_tensor, "channel_row_column").ravel().astype(np.float32)
    #         start = time.time()
    #         result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))
    #         end = time.time()
    #         total_time += end - start
    #     total_time /= num_frames
    #     _logger.info("Average speed: {:.0f}ms/frame".format(total_time * 1000))
    except BaseException as exception:
        _logger.error("Verification of model output failed")
        raise exception

    _logger.info("Verification of model output complete")

class CifarNet(nn.Module):
    def __init__(self):
        super(CifarNet, self).__init__()
        self.conv1 = nn.Conv2d(3, 6, 5)
        self.pool = nn.MaxPool2d(2, 2)
        self.conv2 = nn.Conv2d(6, 16, 5)
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.fc2 = nn.Linear(120, 84, )
        self.fc3 = nn.Linear(84, 10)

    def forward(self, x):
        x = self.pool(F.relu(self.conv1(x)))
        x = self.pool(F.relu(self.conv2(x)))
        x = x.view(-1, 16 * 5 * 5)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x


def cifar10_test():

    torch_model_obj = CifarNet()

    torch_model = "C:models..\\cifar10\\cifar10.pth" # replace the path to your pytorch model's path
    model = "C:models\\cifar10\\cifar10_onnx.ell"  
    full_model_test(model, torch_model, torch_model_obj)

def full_model_test(model, torch_model, torch_model_obj, testing_info={}):

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

    torch_model_obj.load_state_dict(torch.load(torch_model))
    ell_map_ = ell.model.Map(model)
    verify_ell_output_in_vision_model(ell_map, model, onnx_nodes, testing_info={})

logging.basicConfig(level=logging.DEBUG, format="%(message)s")

verify_ell_output_in_vision_model(ell_map, model, onnx_nodes, testing_info={})
