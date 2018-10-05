import numpy as np
import torch

input_size = 4
x_t = torch.randn(input_size)

hidden_size = 3
# each weight is a stack of 3 weights for (input,reset,hidden) operations
w_i = torch.randn((3,hidden_size,input_size), dtype=torch.float32)
w_h = torch.randn((3,hidden_size,hidden_size))
b_i = torch.randn((3,hidden_size))
b_h = torch.randn((3,hidden_size))

print("// Precomputed weights created by GenerateGRUTest.py")
print("const double x_t[] = {" + ", ".join(str(float(x)) for x in x_t.numpy().ravel()) + "};")
print("const double w_i[] = {" + ", ".join(str(float(x)) for x in w_i.numpy().ravel()) + "};")
print("const double w_h[] = {" + ", ".join(str(float(x)) for x in w_h.numpy().ravel()) + "};")
print("const double b_i[] = {" + ", ".join(str(float(x)) for x in b_i.numpy().ravel()) + "};")
print("const double b_h[] = {" + ", ".join(str(float(x)) for x in b_h.numpy().ravel()) + "};")

# h = previous hidden state
# rt = sigma(W_{ ir } x + b_{ ir } + W_{ hr } h + b_{ hr })
# zt = sigma(W_{ iz } x + b_{ iz } + W_{ hz } h + b_{ hz })
# nt = tanh(W_{ in } x + b_{ in } + rt * (W_{ hn } h + b_{ hn }))
# ht = (1 - zt) * nt + zt * h

def GRU(x_t, h_0):
    igates = w_i.matmul(x_t) + b_i
    hgates = w_h.matmul(h_0) + b_h
    chunked_igates = igates.chunk(3)
    chunked_hgates = hgates.chunk(3)
    input_gate = torch.sigmoid(chunked_igates[0] + chunked_hgates[0])
    reset_gate = torch.sigmoid(chunked_igates[1] + chunked_hgates[1])
    new_gate = torch.tanh(chunked_igates[2] + reset_gate * chunked_hgates[2])
    return new_gate + input_gate * (h_0 - new_gate)

print("// Expected output")
h_t = torch.zeros((hidden_size,))
for i in range(3):
    h_t = GRU(x_t, h_t).squeeze()
    print("const double h_" + str(i+1) + "[] = {" + ", ".join(str(float(x)) for x in h_t.numpy().ravel()) + "};")

print("const double* h_t[] = { h_1, h_2, h_3 };")