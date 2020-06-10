#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_default_models.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import os
import shutil
import argparse

import build_gemm_models

script_path = os.path.dirname(os.path.realpath(__file__))
default_model_dir = os.path.join(script_path, "..", "models")

def make_dirs(base_output_dir, data_types, sizes):
    os.makedirs(base_output_dir, exist_ok=True)
    dir_map = {}
    def add_path(path_dict, base_dir, dir, key):
        path = os.path.join(base_dir, dir)
        path_dict[key] = {
            "path": path
        }
        os.makedirs(path, exist_ok=True)
        return path

    for datatype in data_types:
        datatype_dir = add_path(dir_map, base_output_dir, datatype, datatype)
        for size in sizes:
            size_tag = "{}x{}".format(size, size)
            add_path(dir_map[datatype], datatype_dir, size_tag, size)
    return dir_map

def build_loopnest_models(output_dir_map, data_types, sizes):
    for datatype in data_types:
        for size in sizes:
            output_dir = output_dir_map[datatype][size]["path"]
            build_gemm_models.build_all_models(output_dir=output_dir, 
                                               data_type_str=datatype,
                                               M=size,
                                               N=size,
                                               K=size)

def build_fallback_models(output_dir_map, data_types, sizes):
    for datatype in data_types:
        for size in sizes:
            output_dir = output_dir_map[datatype][size]["path"]
            output_filename = os.path.join(output_dir, "gemmELL.ell")
            build_gemm_models.build_model(output_filename=output_filename,
                                          use_fallback=True,
                                          gemm_impl=0,
                                          data_type_str=datatype,
                                          M=size,
                                          N=size,
                                          K=size)
            blas_filename = os.path.join(output_dir, "gemmBLAS.ell")
            shutil.copy(output_filename, blas_filename)
            mkl_filename = os.path.join(output_dir, "gemmMKL.ell")
            shutil.copy(output_filename, mkl_filename)

if __name__ == "__main__":
    data_types = ["float", "double"]
    sizes = [256]
    dir_map = make_dirs(default_model_dir, data_types, sizes)
    build_fallback_models(dir_map, data_types, sizes)
    build_loopnest_models(dir_map, data_types, sizes)
