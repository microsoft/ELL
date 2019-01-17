###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     serial_reader.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x,  numpy, PySerial
#
###################################################################################################

from threading import Thread, Lock, Condition
import numpy as np
import serial


class SerialReader:
    """ This class wraps the serial library and provides chunked serial values scaled to floating point """
    def __init__(self, scale_factor=1):
        """ Create SerialReader object."""
        self.read_buffer = []
        self.lock = Lock()
        self.cv = Condition(self.lock)
        self.closed = False
        self.read_thread = None
        self.scale_factor = scale_factor

    def open(self, sample_size, serial_port=None, baud_rate=115200):
        """ Open the serial port so it returns chunks of data of the given sample_size
        where data is converted to the expected sample_rate and num_channels
        and then scaled to floating point numbers between -1 and 1.

        sample_size - number of samples to return from read method
        serial_port - serial port to read from
        """
        self.serial_port = serial_port
        self.sample_size = sample_size
        self.baud_rate = baud_rate
        self.stdin_thread = Thread(target=self.read_input, args=())
        self.stdin_thread.daemon = True
        self.stdin_thread.start()

    def read_input(self):
        temp = np.zeros(self.sample_size)
        pos = 0
        with serial.Serial(self.serial_port, self.baud_rate) as ser:
            while not self.closed:
                value = float(ser.readline().strip())
                temp[pos] = value
                pos += 1
                if pos == self.sample_size:
                    # protect access to the shared state
                    self.cv.acquire()
                    try:
                        self.read_buffer += [np.array(temp)]
                        if len(self.read_buffer) == 1:
                            self.cv.notify()
                        pos = 0
                    except:
                        pass
                    self.cv.release()

    def read(self):
        """ Read the next chunk. This method blocks until the full sample_size is available """
        while not self.closed:
            # block until data is ready...
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
                data = np.array(result).astype(float)
                return data * self.scale_factor
        return None

    def close(self):
        """ Close the serial port """
        self.closed = True

    def is_closed(self):
        """ return true if the serial port is closed """
        return self.closed
