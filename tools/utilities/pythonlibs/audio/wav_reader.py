###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     wav_reader.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import audioop
import math
import wave

import numpy as np
import pyaudio

class WavReader:
    def __init__(self, sample_rate=16000, channels=1):
        """ Initialize the wav reader with the type of audio you want returned.
        sample_rate  Rate you want audio converted to (default 16 kHz)
        channels     Number of channels you want output (default 1)
        """
        self.input_stream = None
        self.audio = pyaudio.PyAudio()
        self.wav_file = None
        self.requested_channels = int(channels)
        self.requested_rate = int(sample_rate)
        self.buffer_size = 0
        self.sample_width = 0
        self.read_size = None
        self.dtype = None
    
    def open(self, filename, buffer_size, speaker=None):
        """ open a wav file for reading 
        buffersize   Number of audio samples to return on each read() call
        speaker      Optional output speaker to send converted audio to so you can hear it.
        """
        self.speaker = speaker
        # open a stream on the audio input file.
        self.wav_file = wave.open(filename, "rb")
        self.cvstate = None
        self.read_size = int(buffer_size)
        self.actual_channels = self.wav_file.getnchannels()
        self.actual_rate = self.wav_file.getframerate()
        self.sample_width = self.wav_file.getsampwidth()
        # assumes signed integer used in raw audio, so for example, the max for 16bit is 2^15 (32768)
        self.audio_scale_factor = 1 / pow(2, (8*self.sample_width) - 1)
        if self.requested_rate == 0:
            raise Exception("Requested rate cannot be zero")
        self.buffer_size = int(math.ceil((self.read_size * self.actual_rate) / self.requested_rate))

        # convert int16 data to scaled floats
        if self.sample_width == 1:
            self.dtype = np.int8
        elif self.sample_width == 2:
            self.dtype = np.int16
        elif self.sample_width == 4:
            self.dtype = np.int32
        else:
            raise Exception("Unexpected sample width {}, can only handle 1, 2 or 4 byte audio".format(self.sample_width))

        if speaker:
            # configure output stream to match what we are resampling to...
            audio_format = self.audio.get_format_from_width(self.sample_width)
            speaker.open(audio_format, self.requested_channels, self.requested_rate)

    def read(self):
        """ Reads the next chunk of audio (returns buffer_size provided to open)
        It returns the data converted to floating point numbers between -1 and 1, scaled by the range of
        values possible for the given audio format.
        """

        if self.wav_file is None:
            return None
        data = self.wav_file.readframes(self.buffer_size)   
        if len(data) == 0:
            return None

        if self.actual_channels != self.requested_channels:
            if self.requested_channels == 1:
                data = audioop.tomono(data, self.sample_width, 1, 1)
            else:
                raise Exception("Target number of channels must be 1")
                        
        if self.actual_rate != self.requested_rate:
            # convert the audio to the desired recording rate
            data, self.cvstate = audioop.ratecv(data, self.sample_width, self.requested_channels, 
                self.actual_rate, self.requested_rate, self.cvstate)
            
        if self.speaker:
            self.speaker.write(data)

        remainder = len(data) % self.sample_width
        if remainder != 0:
            data = data[:len(data) - remainder]
        data = np.frombuffer(data, dtype=self.dtype).astype(float)

        # pad the last record with zeros so it is valid output.
        data_size = len(data)
        if data_size < self.read_size:
            bigger = np.zeros((self.read_size))
            bigger[:data_size] = data 
            data = bigger
        elif data_size > self.read_size:
            data = data[:self.read_size] # truncate

        return data * self.audio_scale_factor
    
    def close(self):
        if self.wav_file:
            self.wav_file.close()
            self.wav_file = None
    
    def is_closed(self):
        return self.wav_file is None
