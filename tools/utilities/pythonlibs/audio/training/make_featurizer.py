#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_featurizer.py
#  Authors:  Chris Lovett, Chuck Jacobs
#
#  Requires: Python 3.x
#
###################################################################################################

"""
Utility for creating ELL featurizer models
"""
import argparse
import os

import find_ell_root  # noqa: F401
import ell

# Nodes to generate
# Input
# IIR (optional)
# Buffer (optional)
# Hamming window
# FFT
# FilterBank
# Log (optional)
# DCT (optional)
# Output


def _get_tensor_shape(shape):
    return ell.model.PortMemoryLayout(list(shape))


def _create_model(sample_rate, window_size, input_buffer_size, filterbank_type, filterbank_size,
                  nfft=None, iir_node=False, log_node=False, dct_node=False, power_spec=False,
                  log_delta=1.0, filterbank_nfft=None):
    builder = ell.model.ModelBuilder()
    ell_model = ell.model.Model()

    num_filters = filterbank_size

    # Add input node
    input_shape = _get_tensor_shape((input_buffer_size, ))
    input_node = builder.AddInputNode(ell_model, input_shape, ell.nodes.PortType.smallReal)
    last_node = input_node

    # Add optional IIR filter
    if iir_node:
        # this simulates a simple pre-emphasis filter that matches what python_speech_features does
        # in the fbank function.
        a_coeffs = []  # no feed forward
        b_coeffs = [1.0, -0.97]  # subtract 0.97 of previous input
        port = ell.nodes.PortElements(last_node.GetOutputPort("output"))
        last_node = iir_node = builder.AddIIRFilterNode(ell_model, port, b_coeffs, a_coeffs)
        last_node.SetMetadataValue("iir", "true")

    # Add buffer node
    port = ell.nodes.PortElements(last_node.GetOutputPort("output"))
    last_node = buffer_node = builder.AddBufferNode(ell_model, port, window_size)
    buffer_node.SetMetadataValue("window_size", str(window_size))

    # Add Hamming window
    last_node = builder.AddHammingWindowNode(ell_model, ell.nodes.PortElements(last_node.GetOutputPort("output")))

    # Add FFT
    if nfft:
        last_node = builder.AddFFTNode(ell_model, ell.nodes.PortElements(last_node.GetOutputPort("output")), nfft)
    else:
        last_node = builder.AddFFTNode(ell_model, ell.nodes.PortElements(last_node.GetOutputPort("output")))

    if not filterbank_nfft:
        # default is to just use the size of the fft.
        filterbank_nfft = nfft

    if power_spec:
        fft_size = last_node.GetOutputPort("output").Size()
        square_node = builder.AddUnaryOperationNode(
            ell_model, ell.nodes.PortElements(last_node.GetOutputPort("output")),
            ell.nodes.UnaryOperationType.square)
        denom = builder.AddConstantNode(ell_model, [fft_size], ell.nodes.PortType.smallReal)
        last_node = builder.AddBinaryOperationNode(
            ell_model, ell.nodes.PortElements(square_node.GetOutputPort("output")),
            ell.nodes.PortElements(denom.GetOutputPort("output")),
            ell.nodes.BinaryOperationType.divide)

    # Add filterbank
    port = ell.nodes.PortElements(last_node.GetOutputPort("output"))
    if filterbank_type == "mel":
        last_node = builder.AddMelFilterBankNode(ell_model, port, sample_rate, filterbank_nfft, filterbank_size,
                                                 num_filters)
    elif filterbank_type == "linear":
        last_node = builder.AddLinearFilterBankNode(ell_model, port, sample_rate, filterbank_size, num_filters)

    last_node.SetMetadataValue("sample_rate", str(sample_rate))
    last_node.SetMetadataValue("filterbank_size", str(filterbank_size))
    last_node.SetMetadataValue("filterbank_type", filterbank_type)
    last_node.SetMetadataValue("num_filters", str(num_filters))

    # Add optional Log
    if log_node:
        ones_node = builder.AddConstantNode(ell_model, [log_delta] * num_filters, ell.nodes.PortType.smallReal)
        left_port = ell.nodes.PortElements(last_node.GetOutputPort("output"))
        right_port = ell.nodes.PortElements(ones_node.GetOutputPort("output"))
        last_node = builder.AddBinaryOperationNode(ell_model, left_port, right_port,
                                                   ell.nodes.BinaryOperationType.add)
        last_node = builder.AddUnaryOperationNode(ell_model, ell.nodes.PortElements(last_node.GetOutputPort("output")),
                                                  ell.nodes.UnaryOperationType.log)
        last_node.SetMetadataValue("log", "true")

    # Add optional DCT
    if dct_node:
        last_output = last_node.GetOutputPort("output")
        last_node = builder.AddDCTNode(ell_model, ell.nodes.PortElements(last_output), last_output.Size())
        last_node.SetMetadataValue("dct", "true")

    output_shape = _get_tensor_shape((last_node.GetOutputPort("output").Size(), ))
    port = ell.nodes.PortElements(last_node.GetOutputPort("output"))
    output_node = builder.AddOutputNode(ell_model, output_shape, port)

    casted_input_node = ell.nodes.InputNode(input_node)
    output_elements = ell.nodes.PortElements(output_node.GetOutputPort("output"))
    ell_map = ell.model.Map(ell_model, casted_input_node, output_elements)
    return ell_map


def make_featurizer(output_filename, sample_rate, window_size, input_buffer_size, filterbank_type,
                    filterbank_size, nfft=None, iir_node=False, log_node=False, dct_node=False, power_spec=False,
                    log_delta=1.0, filterbank_nfft=None):
    """
    Create a new featurizer ELL model:
    output_filename     - the output ELL model file name
    sample_rate         - the sample rate of the audio
    window_size         - the featurizer input window size
    input_buffer_size   - the size of the buffer node to use
    filterbank_type     - the type of filter bank to use (e.g. 'mel', 'linear' or 'none')
    nfft                - the size of the FFT (defaults to input size)
    filterbank_size     - the number of filters to use
    iir_node            - whether to insert an IIR filter node
    log_node            - whether to append a logarithm node
    dct_node            - whether to include a DCT node in the model
    filterbank_nfft     - allows you to override the nfft parameter on the MelFilterBank (for testing only)
    """
    # Create output directory if necessary
    output_directory = os.path.dirname(output_filename)
    if output_directory and not os.path.isdir(output_directory):
        os.makedirs(output_directory)

    map = _create_model(sample_rate, window_size, input_buffer_size, filterbank_type,
                        filterbank_size, nfft, iir_node, log_node, dct_node, power_spec, log_delta,
                        filterbank_nfft)

    # print("Saving model {}".format(output_filename))
    map.Save(output_filename)
    print("Saved {}".format(output_filename))
    return output_filename


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Create ELL featurization model")

    # options
    arg_parser.add_argument("--output_filename", "-o", help="Output model filename (default 'featurizer.ell')",
                            default="featurizer.ell")
    arg_parser.add_argument("--sample_rate", "-r", help="Sample rate of input", type=int, default=16000)
    arg_parser.add_argument("--window_size", "-ws", help="Number of samples per FFT frame", type=int, default=512)
    arg_parser.add_argument("--input_buffer_size", "-ib", help="Number of samples per input buffer, this allows you \
to get overlapping FFT outputs by shifting the previous data left in the window by this amount.  The result is a \
higher speed classifier that might be better at spotting word boundaries, at the cost of requiring more CPU time",
                            type=int, default=160)
    arg_parser.add_argument("--filterbank_type", "-t", help="Type of filterbank (mel, linear, none)", default="mel")
    arg_parser.add_argument("--filterbank_size", "-fs", help="Number of filters to use in filterbank", type=int,
                            default=40)
    arg_parser.add_argument("--filterbank_nfft", "-fn", help="nfft number for filterbank to use", type=int,
                            default=None)
    arg_parser.add_argument("--nfft", type=int, help="The size of the fft (defaults to input size)", default=None)
    arg_parser.add_argument("--iir", help="Include IIR prefilter", action="store_true")
    arg_parser.add_argument("--log", help="Include a LOG node on the output", action="store_true")
    arg_parser.add_argument("--log_delta", type=float, default=1.0,
                            help="Add this delta before applying the log function")
    arg_parser.add_argument("--dct", help="Add DCT of output", action="store_true")
    arg_parser.add_argument("--power_spec", help="Add a power spectrum scaling of FFT output", action="store_true")

    args = arg_parser.parse_args()

    make_featurizer(args.output_filename, args.sample_rate, args.window_size, args.input_buffer_size,
                    args.filterbank_type, args.filterbank_size, args.nfft, args.iir, args.log, args.dct,
                    args.power_spec, args.log_delta, args.filterbank_nfft)
