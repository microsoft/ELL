
import numpy as np
import torch
hidden_size = 4
input_size = 10
iterations = 3
x_t = torch.randn(input_size)

# each weight is a stack of 3 weights for (input,reset,hidden) operations
w_i = torch.randn((4,hidden_size,input_size), dtype=torch.float32)
w_h = torch.randn((4,hidden_size,hidden_size))
b_i = torch.randn((4,hidden_size))
b_h = torch.randn((4,hidden_size))

print("// Precomputed weights created by GenerateLSGMTest.py")
print("const double x_t[] = {" + ", ".join(str(float(x)) for x in x_t.numpy().ravel()) + "};")
print("const double w_i[] = {" + ", ".join(str(float(x)) for x in w_i.numpy().ravel()) + "};")
print("const double w_h[] = {" + ", ".join(str(float(x)) for x in w_h.numpy().ravel()) + "};")
print("const double b_i[] = {" + ", ".join(str(float(x)) for x in b_i.numpy().ravel()) + "};")
print("const double b_h[] = {" + ", ".join(str(float(x)) for x in b_h.numpy().ravel()) + "};")


def LSTM(x_t, c_t, h_t):
    igates = w_i.matmul(x_t) + b_i
    hgates = w_h.matmul(h_t) + b_h
    chunked_igates = igates.chunk(4)
    chunked_hgates = hgates.chunk(4)
    it = torch.sigmoid(chunked_igates[0] + chunked_hgates[0])
    ft = torch.sigmoid(chunked_igates[1] + chunked_hgates[1])
    gt = torch.tanh(chunked_igates[2] + chunked_hgates[2])
    ot = torch.sigmoid(chunked_igates[3] + chunked_hgates[3])
    c_t = (ft * c_t) + (it * gt)
    h_t = ot * torch.tanh(c_t)
    return (c_t.squeeze(), h_t.squeeze())

h_t = torch.zeros((hidden_size,))
c_t = torch.zeros((hidden_size,))

print("// Expected output")
h_t = torch.zeros((hidden_size,))
for i in range(iterations):
    c_t, h_t = LSTM(x_t, c_t, h_t)
    print("const double h_" + str(i+1) + "[] = {" + ", ".join(str(float(x)) for x in h_t.numpy().ravel()) + "};")

print("const double* h_t[] = { h_1, h_2, h_3 };")