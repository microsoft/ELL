import numpy as np
import torch

input_size = 10
x_t = torch.randn(input_size)

hidden_size = 3

w_i = torch.randn((hidden_size, input_size), dtype=torch.float32)
w_h = torch.randn((hidden_size, hidden_size), dtype=torch.float32)
b_i = torch.randn((hidden_size), dtype=torch.float32)
b_h = torch.randn((hidden_size), dtype=torch.float32)

print("// Precomputed weights created by GenerateGRUTest.py")
print("const double x_t[] = {" + ", ".join(str(float(x)) for x in x_t.numpy().ravel()) + "};")
print("const double w_i[] = {" + ", ".join(str(float(x)) for x in w_i.numpy().ravel()) + "};")
print("const double w_h[] = {" + ", ".join(str(float(x)) for x in w_h.numpy().ravel()) + "};")
print("const double b_i[] = {" + ", ".join(str(float(x)) for x in b_i.numpy().ravel()) + "};")
print("const double b_h[] = {" + ", ".join(str(float(x)) for x in b_h.numpy().ravel()) + "};")

# h_t = {tanh}(w_{ih} x_t + b_{ih} + w_{hh} h_{(t-1)} + b_{hh})

# so if we concatenate the weights for input + hidden 
# x_h = x + h_t
# h_t = tanh(W x_h + b )

def RNN(x_t, h_0):
    input_gate = w_i.matmul(x_t) + b_i
    hidden_gate = w_h.matmul(h_0) + b_h
    new_gate = torch.tanh(input_gate + hidden_gate)
    return new_gate

print("// Expected output")
h_t = torch.zeros((hidden_size,))
for i in range(3):
    h_t = RNN(x_t, h_t).squeeze()
    print("const double h_" + str(i+1) + "[] = {" + ", ".join(str(float(x)) for x in h_t.numpy().ravel()) + "};")

print("const double* h_t[] = { h_1, h_2, h_3 };")