import os
import sys
import logging
import time

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
import mnist as mnist

_logger = logging.getLogger(__name__)


# A torch model with only a conv layer
class Net(nn.Module): 
        def __init__(self):
            super(Net, self).__init__()
            self.conv0 = nn.Conv2d(3, 10, kernel_size=5, stride=1, bias=True, padding=0)
            self.conv1 = nn.Conv2d(10, 20, kernel_size=5, stride=1, bias=True, padding=1)
            self.conv2_drop = nn.Dropout2d()
            self.bn1 = nn.BatchNorm2d(20)
            self.fc1 = nn.Linear(320, 50)
            self.fc2 = nn.Linear(50, 10)

            nn.init.uniform_(self.conv1.bias)
            nn.init.uniform_(self.bn1.bias)


        def forward(self, x):
            x1 = F.max_pool2d(self.conv0(x), 2)
            x1 = self.conv1(x1)
            x1 = self.conv2_drop(x1)
            x1 = x1.view(-1, 320)
            # x2 = self.bn1(x1)
            # x = F.relu(x)
            # x = F.leaky_relu(x2)
            # x = torch.cat((x1,x2), 1)
            x1 = self.fc1(x1)
            # x1 = self.fc2(x1)
            x = x1
            return x

model = Net()
tensor = torch.randn(1, 3, 32, 32)
model.forward(tensor)
_logger.info("Exporting model to ONNX")
torch_out = torch.onnx._export(model, tensor, "model.onnx", export_params=True, verbose=True)

# Testing
ell_map = onnx_converter("model.onnx")
ell_input_tensor = memory_shapes.get_tensor_in_ell_order(tensor.numpy(), "filter_channel_row_column").ravel().astype(np.float32)

ell_out = ell_map.Compute(ell_input_tensor, dtype=np.float32)
np.testing.assert_almost_equal(torch_out.data.cpu().numpy(), ell_out, decimal=3)


def verify_ell_output_in_vision_model(ell_map, torch_model, testing_info):
    _logger.info("Verification of model output starting")
    try:
        torch_model.eval()
        ell_input_shape = ell_map.GetInputShape()
        ell_shape = (ell_input_shape.rows, ell_input_shape.columns, ell_input_shape.channels)
        pytorch_input_shape =  (ell_shape[2], ell_shape[0], ell_shape[1])
        
        torch_input_tensor = np.random.random((1, pytorch_input_shape[0], pytorch_input_shape[1], pytorch_input_shape[2])) * 255
        torch_input_tensor = np.float32(torch_input_tensor) 
        order = "channel"
        if len(torch_input_tensor.shape) == 4:
            order = "filter_channel_row_column"
        elif len(torch_input_tensor.shape) == 3:
            order = "channel_row_column"

        print("Checking shapes: torch shape =", torch_input_tensor.shape)
        print("Order", order)
        ell_input_tensor = memory_shapes.get_tensor_in_ell_order(torch_input_tensor, order).ravel().astype(np.float32)
        ell_input_tensor = np.float32(ell_input_tensor)

        

        # Get output from PyTorch model
        _logger.info("Getting PyTorch results")
        with torch.no_grad():
            torch_output = torch_model.forward(torch.from_numpy(torch_input_tensor).float())
            order = "channel"
            if len(torch_output.shape) == 4:
                order = "filter_channel_row_column"
            elif len(torch_output.shape) == 3:
                order = "channel_row_column" 
            torch_output = memory_shapes.get_tensor_in_ell_order(torch_output.numpy(), order)

        print("Check shapes: Ell input shape =", ell_input_tensor.shape)
        # Get computed ELL result
        _logger.info("Getting computed ELL results")
        result_from_compute = np.array(ell_map.Compute(ell_input_tensor, dtype=np.float32))
        result_from_compute = result_from_compute.reshape((1, 1, 10))
        


        # Get compiled ELL result
        _logger.info("Getting compiled ELL results")
        compiler_options = ell.model.MapCompilerOptions()
        compiler_options.useBlas = True
        compiled_ell_map = ell_map.Compile("host", "model", "predict", compilerOptions=compiler_options, dtype=np.float32)

        result_from_compiled = np.array(compiled_ell_map.Compute(ell_input_tensor, dtype=np.float32))
        print("Check shapes: torch shape = ", torch_output.shape, "ell shape = ", result_from_compute.shape)
        # Verify the computed result against the pytorch result
        # np.testing.assert_array_almost_equal(
        #    torch_output, result_from_compute, decimal=4, err_msg=(
        #        'results for computed ELL model do not match ONNX output!'))
        # _logger.info("Verified computed result against ONNX")
            
        # Verify the compiled result  against the onnx result
        np.testing.assert_array_almost_equal(
            torch_output, result_from_compiled, decimal=4, err_msg=(
                'results for compiled ELL model do not match ONNX output!'))
        _logger.info("Verified compiled result against ONNX")
        
        # # Verify the compiled result agrees with the computed result
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

torch_model = mnist.Net()

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

# torch_model = CifarNet()

# torch_model.load_state_dict(torch.load("../models/mnist.pth"))
# ell_map_ = ell.model.Map("../models/mnist_pth.ell")
# torch_model.load_state_dict(torch.load("C:\\Users\\v-iltien\\Desktop\\train\\pytorch\\cifar10\\cifar10.pth"))
# ell_map_ = ell.model.Map("C:\\Users\\v-iltien\\Desktop\\train\\pytorch\\cifar10\\cifar10_onnx.ell")


logging.basicConfig(level=logging.DEBUG, format="%(message)s")

verify_ell_output_in_vision_model(ell_map, model, testing_info={})