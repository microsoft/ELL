#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     build_gemm_models.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import os
import sys

script_path = os.path.dirname(os.path.realpath(__file__))
sys.path += [os.path.join(script_path, "../../../pythonlibs")]

import find_ell

ell_build_root = find_ell.find_ell_build()
sys.path += [os.path.join(ell_build_root, "interfaces", "python", "package")]

import ell
import random
import argparse
import numpy as np

type_mapping = {
    "double": ell.nodes.PortType.real,
    "float": ell.nodes.PortType.smallReal
}

def make_matrix(rows, cols, use_fixed_seed, data_type_str):
    if use_fixed_seed:
        np.random.seed(0)
    return ell.math.DoubleVector(np.random.rand(rows * cols))

def build_model(output_filename, use_fallback, gemm_impl, M=256, N=256, K=256, kernel_size=[1, 1, 1], cache_sizes=[64, 64, 64], data_type_str="double", ignore_correctness=False):
    model = ell.model.Model()
    mb = ell.model.ModelBuilder()

    data_type = type_mapping[data_type_str]

    # input node is the left matrix in the matrix multiplication
    input_layout = ell.model.PortMemoryLayout([M, K])
    input_node = mb.AddInputNode(model, input_layout, data_type)
    input_node_output = ell.nodes.PortElements(input_node.GetOutputPort("output"))

    realign_node = mb.AddOutputNode(model, input_layout, input_node_output)
    realign_node_output = ell.nodes.PortElements(realign_node.GetOutputPort("output"))

    right_matrix = make_matrix(K, N, not ignore_correctness, data_type_str)
    right_constant_memory_layout = ell.model.PortMemoryLayout([K, N])
    right_constant_node = mb.AddConstantNode(model, right_matrix, right_constant_memory_layout, data_type)
    right_node_output = ell.nodes.PortElements(right_constant_node.GetOutputPort("output"))

    gemm_output = None
    if use_fallback:
        fallback_gemm_node = mb.AddMatrixMatrixMultiplyNode(model, realign_node_output, right_node_output)
        gemm_output = ell.nodes.PortElements(fallback_gemm_node.GetOutputPort("output"))
    else:
        gemm_node = mb.AddMatrixMatrixMultiplyCodeNode(model, realign_node_output, right_node_output, cache_sizes[0], cache_sizes[1], cache_sizes[2], kernel_size[0], kernel_size[1], kernel_size[2], gemm_impl)
        gemm_output = ell.nodes.PortElements(gemm_node.GetOutputPort("output"))

    # add output node
    output_node = mb.AddOutputNode(model, ell.model.PortMemoryLayout([M, N]), gemm_output)

    ell_map = ell.model.Map(model, input_node, ell.nodes.PortElements(output_node.GetOutputPort("output")))
    ell_map.Save(output_filename)

def build_all_models(output_dir, M=256, N=256, K=256, kernel_size=[4, 4, 4], cache_sizes=[64, 64, 64], data_type_str="double", base_filename="gemm", file_extension="ell", ignore_correctness=False):
    gemm_impl_count = ell.nodes.MatrixMatrixMultiplyImplementation.ImplementationCount
    for gemm_impl in range(gemm_impl_count):
        output_filename = os.path.join(output_dir, "{}{}.{}".format(base_filename, gemm_impl, file_extension))
        build_model(output_filename=output_filename,
                    use_fallback=False,
                    gemm_impl=gemm_impl,
                    M=M,
                    N=N,
                    K=K,
                    kernel_size=kernel_size,
                    cache_sizes=cache_sizes,
                    data_type_str=data_type_str,
                    ignore_correctness=ignore_correctness)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output_dir", required=True)
    parser.add_argument("-M", type=int, default=256)
    parser.add_argument("-N", type=int, default=256)
    parser.add_argument("-K", type=int, default=256)
    parser.add_argument("--type", "-t", choices=list(type_mapping), default="double")
    parser.add_argument("--ignore_correctness", help="Don't use a fixed random seed. A fixed random seed is used to validate GEMM results between different implementations", action="store_true")
    parser.add_argument("--panel_size", "-ps", type=int, nargs=3, default=[64, 64, 64], help="Panel size values for M, N, and K dimensions")
    parser.add_argument("--kernel_size", "-ks", type=int, nargs=3, default=[1, 1, 1], help="Kernel size values for M, N, and K dimensions")
    parser.add_argument("--base_filename", default="gemm")
    parser.add_argument("--file_extension", default="ell")

    group = parser.add_mutually_exclusive_group()
    group.add_argument("--all_impls", "-a", action="store_true")
    group.add_argument("--impl", "-i", type=int, default=0)
    group.add_argument("--fallback", "-f", action="store_true", help="Use ELL naive for-loops or BLAS impl via MatrixMultiplyNode (non-code-node)")

    args = parser.parse_args()

    if args.all_impls:
        build_all_models(output_dir=args.output_dir,
                         M=args.M,
                         N=args.N,
                         K=args.K,
                         kernel_size=args.kernel_size,
                         cache_sizes=args.panel_size,
                         data_type_str=args.type,
                         base_filename=args.base_filename,
                         file_extension=args.file_extension,
                         ignore_correctness=args.ignore_correctness)
    else:
        output_filename = os.path.join(args.output_dir, "{}.{}".format(args.base_filename, args.file_extension))
        build_model(output_filename=output_filename,
                    use_fallback=args.fallback,
                    gemm_impl=args.impl,
                    M=args.M,
                    N=args.N,
                    K=args.K,
                    kernel_size=args.kernel_size,
                    cache_sizes=args.panel_size,
                    data_type_str=args.type,
                    ignore_correctness=args.ignore_correctness)
