#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     play_audio.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import os
import sys
import argparse

import wav_reader
import speaker

# this is a test script to show how to use WavReader and Speaker classes.

arg_parser = argparse.ArgumentParser(description="Play an audio file after resampling it")
arg_parser.add_argument("filename", help="wav file to play ")
arg_parser.add_argument("--sample_rate", "-s", help="Audio sample rate to use", default=16000, type=int)
arg_parser.add_argument("--channels", "-c", help="Audio channels to use", default=1, type=int)

args = arg_parser.parse_args()

# First tell the WavReader what sample rate and channels we want the audio converted to
reader = wav_reader.WavReader(args.sample_rate, args.channels)

# Create a speaker object which we will give to the WavReader.  The WavReader will pass 
# the re-sampled audio to the Speaker so you can hear what it sounds like
speaker = speaker.Speaker()

# open the reader asking for 256 size chunks of audio, converted to floating point betweeo -1 and 1.
reader.open(args.filename, 256, speaker)

print("wav file contains sample rate {} and {} channels".format(reader.actual_rate, reader.actual_channels))

# pump the reader until it returns None.  In a real app you would assign the results of read() to 
# a variable so you can process the audio chunks returned.
while reader.read() is not None:
    pass
