#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     record.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import argparse
import time
import wave

import numpy as np
import microphone


def main(rate, channels, duration):
    sample_width = 2  # 16 bit

    mic = microphone.Microphone(auto_scale=False)
    mic.open(1024, rate, channels)

    start = time.time()

    print("Recording for {} seconds...".format(duration))

    with wave.open("audio.wav", "wb") as writer:
        writer.setnchannels(channels)
        writer.setsampwidth(sample_width)
        writer.setframerate(rate)
        while time.time() < start + duration:
            buffer = mic.read()
            int16 = buffer.astype(np.int16)
            writer.writeframes(int16)

    print("finished, press ctrl+c")


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Test a feature model and optional classifier")
    arg_parser.add_argument("--sample_rate", "-r", help="The sample rate for the recording", default=16000, type=int)
    arg_parser.add_argument("--channels", "-c", help="The number of channels to record", default=1, type=int)
    arg_parser.add_argument("--duration", "-d", help="The number of seconds to record", default=1, type=int)
    arg_parser.add_argument("--list_devices", help="List available input devices", action="store_true")
    arg_parser.add_argument("--input_device", "-i", help="Index of input device (see --list_devices)",
                            default=1, type=int)
    args = arg_parser.parse_args()
    if args.list_devices:
        microphone.list_devices()
    else:
        main(args.sample_rate, args.channels, args.duration)
