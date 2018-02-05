###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     audioConverter.py
#  Authors:  Chris Lovett
#
###############################################################################

import sys
import numpy as np
import librosa
import argparse

def display_spectrogram(data):
    """
    Display given mel spectrogram using matplotlib (used  when --debug option is passed)
    """
    import matplotlib.pyplot as plt
    import librosa.display
    plt.subplot(1, 1, 1)
    librosa.display.specshow(data, y_axis='linear')
    plt.colorbar(format='%+2.0f dB')
    plt.title('Linear-frequency power spectrogram')
    plt.show()

def main(argv) :
    """
    This is a plugin for the debugCompiler tool.  It expects a list of string arguments as input and it returns a wav
    audio file preprocessed with log mel spectrogram filtering.
    """
    arg_parser = argparse.ArgumentParser("audioConverter takes a wav file as input and converts it to an array of floating point numbers "
        "representing the log mel spectrogram filtering of that audio file")
    arg_parser.add_argument("--window", type=int, help="size of the window (default 128)", default = 128)
    arg_parser.add_argument("--shift", type=int, help="number of samples between successive frames (default 32)", default = 32)
    arg_parser.add_argument("--channels", type=int, help="number of frequency channels (default 40)", default = 40)
    arg_parser.add_argument("--wav", help="name of file containing audio wav data (default None)", default = None, required = True)
    arg_parser.add_argument("--debug", help="display the result (default False)",  action="store_true")

    args = arg_parser.parse_args(argv)

    aud, sr = librosa.load(args.wav)
    if aud is None:
        print("Error reading wav file {}".format(args.wav))
        return []

    data = librosa.logamplitude(librosa.feature.melspectrogram(aud, sr=sr, n_fft=args.window, hop_length=args.shift, n_mels=args.channels), ref_power=1.0)

    print("loaded spectrogram shape is {}".format(data.shape))
    
    if args.debug:
        display_spectrogram(data)

    return list(data)


if __name__ == "__main__":
    args = sys.argv
    main(args[1:])
