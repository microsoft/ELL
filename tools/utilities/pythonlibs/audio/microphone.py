###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     microphone.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################

import audioop
import math
import signal
import sys
from threading import Thread, Lock, Condition

import numpy as np
import pyaudio

class Microphone:
    """ This class wraps the pyaudio library and it's input stream callback providing a simple to
    use Microphone class that you can simply read from """
    def __init__(self, console=True):        
        """ Create Microphone object.
        console - specifies whether you are running from console app, if so this will listen for
        stdin "x" so user can tell you app to close the microphone """
        self.audio = pyaudio.PyAudio()
        self.read_buffer = []
        self.lock = Lock()
        self.cv = Condition(self.lock)
        self.closed = False
        self.num_channels = 1
        self.console = console
        self.stdin_thread = None
        self.input_stream = None
    
    def open(self, sample_size, sample_rate, num_channels, input_device=None):
        """ Open the microphone so it returns chunks of audio samples of the given sample_size
        where audio is converted to the expected sample_rate and num_channels
        and then scaled to floating point numbers between -1 and 1.
        
        sample_size - number of samples to return from read method
        audio_scale_factor - audio is converted to floating point using this scale
        sample_rate - the expected sample rate (e.g. 16000)
        num_channels - the number of audio channels to return
        input_device - input device index if you don't want to use the default
        """
        self.sample_rate = sample_rate
        self.sample_size = sample_size
        self.num_channels = num_channels
        self.audio_format = pyaudio.paInt16
        self.cvstate = None
        if input_device:
            info = self.audio.get_device_info_by_index(input_device)
        else:
            info = self.audio.get_default_input_device_info()
        self.mic_rate = int(info['defaultSampleRate'])
        buffer_size = int(math.ceil(sample_size * self.mic_rate / sample_rate))
        self.input_stream = self.audio.open(format=pyaudio.paInt16,
                        channels=num_channels,
                        rate=self.mic_rate, 
                        input=True,
                        frames_per_buffer=buffer_size,
                        stream_callback=self._on_recording_callback)    
        self.audio_scale_factor = 1 / 32768  # since we are using pyaudio.paInt16.
        self.closed = False
        if self.console:
            # since our read call blocks the UI we use a separate thread to monitor user input
            self.stdin_thread = Thread(target=self.monitor_input, args=(sys.stdin,))        
            self.stdin_thread.daemon = True
            self.stdin_thread.start()
    
    def _on_recording_callback(self, data, frame_count, time_info, status): 
        # convert the incoming audio to the desired recording rate
        result, self.cvstate = audioop.ratecv(data, 2, 
            self.num_channels, self.mic_rate, self.sample_rate, self.cvstate)
            
        # protect access to the shared state
        self.cv.acquire()
        try:    
            self.read_buffer += [ result ]
            if len(self.read_buffer) == 1:
                self.cv.notify()
        except:
            pass
        self.cv.release()
        return (data, pyaudio.paContinue)

    def read(self):
        """ Read the next audio chunk. This method blocks until the audio is available """
        while not self.closed:
            # block until microphone data is ready...         
            result = None            
            self.cv.acquire()            
            try:    
                while len(self.read_buffer) == 0:
                    if self.closed:
                        return None
                    self.cv.wait(0.1)             
                result = self.read_buffer.pop(0)
            except:
                pass
            self.cv.release()
            
            if result is not None:
                # convert int16 data to scaled floats
                data = np.frombuffer(result, dtype=np.int16)
                data = data.astype(float)
                if len(data) < self.sample_size:
                    # pad the last record with zeros so it is valid input also.
                    bigger = np.zeros((self.sample_size))
                    bigger[:len(data)] = data 
                    data = bigger
                elif len(data) > self.sample_size:
                    data = data[:self.sample_size] # just truncate it, might be off by 1 due to rounding errors

                return data * self.audio_scale_factor
        return None

    def close(self):
        """ Close the microphone """
        self.closed = True
        if self.input_stream:
            self.input_stream.close()

    def is_closed(self):
        """ return true if the microphone is closed """
        return self.closed


    def monitor_input(self, stream):
        """ monitor stdin since our read call is blocking, this way user can type 'x' to quit """
        try:
            while not self.closed:
                out = stream.readline()
                if out:
                    msg = out.rstrip('\n')
                    if msg == "exit" or msg == "quit" or msg == "x":                        
                        print("closing microphone...")
                        self.closed = True
                else:
                    break
        except:
            errorType, value, traceback = sys.exc_info()
            msg = "### Exception: %s: %s" % (str(errorType), str(value))
            self.print(msg)