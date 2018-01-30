#!/usr/bin/env python3
""" Script to generate data files used as ground truth for dsp tests

Writes a C++ file with a set of functions of the form:

std::vector<double> GetData()
{
    return { 1, 2, 3 };
} 
"""

from string import Template
import os
import numbers

import numpy as np
import scipy
import scipy.signal as dsp
import librosa

__this_dir = os.path.dirname(__file__)

__file_header = Template("""
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPTestData.${ext} (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

""")

#
# Handy "file" wrapper
#
class c_file(object):
    def __init__(self, filename):
        """ type is either "h", "cpp", or "auto" """
        self.filebase, self.ext = os.path.splitext(filename)
        self.fp = open(filename, "w")

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.fp.close()

    def is_header(self):
        return self.ext == ".h"

    def is_src(self):
        return not self.is_header()

    def extension(self):
        return self.ext[1:]

    def write(self, *args, **kwargs):
        self.fp.write(*args, **kwargs)

#
# Utility functions
#
def cstr(data):
    if isinstance(data, numbers.Real):
        return str(data)
    elif isinstance(data, numbers.Complex):
        return Template("{${re}, ${im}}").substitute(re=data.real, im=data.imag)
    else:
        return "{" + ", ".join(cstr(x) for x in data) + "}"

def get_return_type(data):
    if isinstance(data, numbers.Number):
        return "double"
    else:
        return "std::vector<" + get_return_type(data[0]) + ">"

def write_data_as_std_vector(fp, data):
    """ for a 1D array, this should be `{ 1, 2, 3, ... }` """
    fp.write(cstr(data))
    fp.write(";\n")

def write_file_header(code_file):
    code_file.write(__file_header.substitute(ext=code_file.extension()))
    if code_file.is_header():
        code_file.write("#pragma once\n\n#include <vector>\n\n")
    else:
        code_file.write("#include \"DSPTestData.h\"\n\n") 

def write_function_body_or_decl(code_file, function_name, data):
    return_type = get_return_type(data)
    decl = Template("${return_type} ${function_name}()").substitute(return_type=return_type, function_name=function_name)
    code_file.write(decl)

    if code_file.is_header():
        code_file.write(";\n")
    else:
        # Write function body
        code_file.write("\n{\n    return ")
        write_data_as_std_vector(code_file, data)
        code_file.write("}\n\n")

#
# Specific datatypes
#

# Input data
def get_spectrum_test_data(size):
    np.random.seed(123 + size)
    noise = 0.5 * np.random.rand(size)
    sin1 = np.sin(np.linspace(0, 2*np.pi, size))
    sin2 = 0.3 * np.sin(np.pi*0.1 + np.linspace(0, 2*np.pi*10, size))
    sin3 = 0.7 * np.sin(np.pi*0.25 + np.linspace(0, 2*np.pi*12.3, size)) # aperiodic
    return noise + sin1 + sin2 + sin3

def write_fft_test_data_functions(code_file):
    function_template = Template("GetFFTTestData_${size}")
    for size in (32, 64, 128, 256, 512, 1024):
        np.random.seed(123 + size)
        data = get_spectrum_test_data(size)
        function_name = function_template.substitute(size=size)
        write_function_body_or_decl(code_file, function_name, data)

# DCT-specific
def write_dct_functions(code_file): 
    function_template_II = Template("GetDCTReference_II_${window_size}")
    function_template_II_norm = Template("GetDCTReference_II_norm_${window_size}")
    function_template_III = Template("GetDCTReference_III_${window_size}_${num_filters}")
    
    # scipy's dct performs a DCT on the input array, so we call it on the identity matrix to get
    # the equivalent matrix coefficients. The scipy implemention is also different from ours by a factor
    # of 2, so we divide the result in half here. 
    for window_size in (8, 64, 128):
        identity = np.eye(window_size)
        function_name_II = function_template_II.substitute(window_size=window_size)
        function_name_II_norm = function_template_II_norm.substitute(window_size=window_size)
        dct_II_data = (scipy.fftpack.dct(identity, type=2) / 2.0).transpose()
        dct_II_norm_data = scipy.fftpack.dct(identity, type=2, norm='ortho').transpose()
        write_function_body_or_decl(code_file, function_name_II, dct_II_data)
        write_function_body_or_decl(code_file, function_name_II_norm, dct_II_norm_data)

    # librosa returns a filterbank matrix from its dct function. The librosa implementation is of the DCT-III
    # variation, which we don't currently support
    for window_size in (64, 128):
        for num_filters in (40, 13):
            # dct_filters = librosa.filters.dct(13, 1 + n_fft // 2)
            num_inputs = window_size//2 + 1
            dct_III_data = librosa.filters.dct(num_filters, num_inputs)
            function_name_III = function_template_III.substitute(window_size=window_size, num_filters=num_filters)
            write_function_body_or_decl(code_file, function_name_III, dct_III_data)

# FFT-specific
def write_fft_functions(code_file):
    function_template = Template("GetRealFFT_${size}")
    for size in (32, 64, 128, 256, 512, 1024):
        # Get the requested window and return it from the function
        input_data = get_spectrum_test_data(size)
        data = np.abs(np.fft.rfft(input_data, norm=None))
        function_name = function_template.substitute(size=size)
        write_function_body_or_decl(code_file, function_name, data)

# IIR-specific

# Mel-specific
def write_mel_functions(code_file): 
    function_template = Template("GetMelReference_${sample_rate}_${window_size}_${num_filters}")
    for sample_rate in (16000, 8000):
        for window_size in (512,):
            for num_filters in (128, 40, 13):
                data = librosa.filters.mel(sample_rate, window_size, num_filters, norm=None)
                function_name = function_template.substitute(sample_rate=sample_rate, window_size=window_size, num_filters=num_filters)
                write_function_body_or_decl(code_file, function_name, data)

# Hamming windows
def write_window_functions(code_file): 
    function_template = Template("GetReferenceHammingWindow_${size}_${mode}")
    for sym in (True, False):
        for size in (128, 301, 1000):
            # Get the requested window and return it from the function
            window = dsp.hamming(size, sym=sym)
            function_name = function_template.substitute(size=size, mode='symmetric' if sym else 'periodic')
            write_function_body_or_decl(code_file, function_name, window)

def write_file(code_file):
    write_file_header(code_file)
    write_fft_test_data_functions(code_file)
    write_fft_functions(code_file)
    write_dct_functions(code_file)
    write_mel_functions(code_file)
    write_window_functions(code_file)


#
# Main function
#
if __name__ == "__main__":
    header_filename = os.path.join(__this_dir, "..", "include", "DSPTestData.h")
    with c_file(header_filename) as header_file:
        write_file(header_file)
    src_filename = os.path.join(__this_dir, "..", "src", "DSPTestData.cpp")
    with c_file(src_filename) as src_file:
        write_file(src_file)
