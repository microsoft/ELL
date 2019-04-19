#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     play_audio.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import argparse

import wav_reader
import speaker

# this is a test script to show how to use WavReader and Speaker classes.

arg_parser = argparse.ArgumentParser(description="Play an audio file after resampling it")
arg_parser.add_argument("filename", help="wav file to play ")
arg_parser.add_argument("--sample_rate", "-s", help="Audio sample rate to use", default=16000, type=int)
arg_parser.add_argument("--channels", "-c", help="Audio channels to use", default=1, type=int)
arg_parser.add_argument("--buffer_size", help="Read buffer size", default=512, type=int)
arg_parser.add_argument("--code", help="Output c-code for sample data", action="store_true")

args = arg_parser.parse_args()

# First tell the WavReader what sample rate and channels we want the audio converted to
reader = wav_reader.WavReader(args.sample_rate, args.channels, auto_scale=False)

# Create a speaker object which we will give to the WavReader.  The WavReader will pass
# the re-sampled audio to the Speaker so you can hear what it sounds like
speaker = speaker.Speaker()

# open the reader asking for size chunks of audio, converted to floating point between -1 and 1.
reader.open(args.filename, args.buffer_size, speaker)

print("wav file contains sample rate {} and {} channels".format(reader.actual_rate, reader.actual_channels))

code = args.code
# pump the reader until it returns None.  In a real app you would assign the results of read() to
# a variable so you can process the audio chunks returned.
while True:
    buffer = reader.read()
    if buffer is None:
        break
    if code:
        print("{", end='')
        for x in buffer:
            print("{}, ".format(x), end='')
        print("},")
