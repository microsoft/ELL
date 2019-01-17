#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     vad.py
#  Authors:  Chris Lovett, Ivan Tashev
#
#  Requires: Python 3.x
#
###################################################################################################

import numpy as np
import math

DEFAULT_TAU_UP = 1.54
DEFAULT_TAU_DOWN = 0.074326
DEFAULT_THRESHOLD_UP = 3.552713
DEFAULT_THRESHOLD_DOWN = 0.931252
DEFAULT_LARGE_INPUT = 2.400160
DEFAULT_GAIN_ATT = 0.002885
DEFAULT_LEVEL_THRESHOLD = 0.007885


class VoiceActivityDetector:
    """ This class detects speech activity in an audio stream """
    def __init__(self, sample_rate, window_size):
        """ Create new object, providing the transform, sample rate and window size """
        self.cmw = _CMessageWeights(sample_rate, window_size)
        self.frame_duration = window_size / sample_rate
        self.window_size = window_size
        self.reset()

    def reset(self):
        """ reset the activity detector so it can be used on a new stream """
        self.levels = []
        self.tr = _ActivityTracker()
        self.count = 0
        self.intervals = []
        self.start = None

    def configure(self, tau_up, tau_down, threshold_up, threshold_down, large_input, gain_att, level_threshold):
        self.tr.configure(tau_up, tau_down, threshold_up, threshold_down, large_input, gain_att, level_threshold)

    def process(self, data):
        """ process incoming audio stream, this data should already be floating point in the range [0-1].
        This method returns 1 when it detects activity in the stream and 0 otherwise.
        """
        if len(data) != self.window_size:
            raise Exception("data length should match window_size")
        level = 0
        for i in range(self.window_size):
            level += data[i] * self.cmw.get_weight(i)

        level = level / self.window_size
        self.level = level

        t = self.count * self.frame_duration
        signal = self.tr.classify(t, level)
        if signal > 0 and self.start is None:
            # start of signal
            self.start = self.count
        elif signal == 0 and self.start is not None:
            # end of signal
            self.intervals += [(self.start, self.count)]
            self.start = None
        self.count += 1
        self.signal = signal
        return signal


class _ActivityTracker:
    """ internal use only - this implements the activity detection state machine """
    def __init__(self):
        self.tau_up = DEFAULT_TAU_UP
        self.tau_down = DEFAULT_TAU_DOWN
        self.threshold_up = DEFAULT_THRESHOLD_UP
        self.threshold_down = DEFAULT_THRESHOLD_DOWN
        self.level = 0.1
        self.last_level = 0
        self.large_input = DEFAULT_LARGE_INPUT
        self.gain_att = DEFAULT_GAIN_ATT
        self.level_threshold = DEFAULT_LEVEL_THRESHOLD
        self.time = 0.0
        self.signal = 0

    def configure(self, tau_up, tau_down, threshold_up, threshold_down, large_input, gain_att, level_threshold):
        self.tau_up = tau_up
        self.tau_down = tau_down
        self.threshold_up = threshold_up
        self.threshold_down = threshold_down
        self.large_input = large_input
        self.gain_att = gain_att
        self.level_threshold = level_threshold

    def _next_level(self, time, level):
        self.last_level = level
        if level < self.level:
            self.level = self.level + (time - self.time) / self.tau_down * (level - self.level)
            if self.level < level:
                self.level = level
        elif level > self.large_input * self.level:
            self.level = self.level + self.gain_att * (time - self.time) / self.tau_up * (level - self.level)
            if self.level > level:
                self.level = level
        else:
            self.level = self.level + (time - self.time) / self.tau_up * (level - self.level)
            if self.level > level:
                self.level = level
        if (level > self.threshold_up * self.level) and (level > self.level_threshold):
            self.signal = 1
        if level < self.threshold_down * self.level:
            self.signal = 0
        self.time = time
        return self.level

    def classify(self, time, level):
        """ compute the next signal state given input time and power levels """
        self._next_level(time, level)
        return self.signal


class _CMessageWeights:
    """ internal use only - this class builds a message weight lookup table for given sample_rate and window_size """
    def __init__(self, sample_rate, window_size):
        self.max_freq = 4000
        self.freq_map = [60, 100, 200, 300, 400, 500, 600, 700, 800, 900,
                         1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
                         2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900,
                         3000, 3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900,
                         4000]
        self.msg_weights = [-54.65, -41.71, -25.17, -16.64, -11.29, -7.55, -4.75, -2.66, -1.19, -0.32,
                            0.03, 0.03, -0.17, -0.44, -0.71, -0.94, -1.12, -1.24, -1.32, -1.36,
                            -1.38, -1.39, -1.41, -1.44, -1.50, -1.60, -1.76, -1.97, -2.26, -2.62,
                            -3.09, -3.66, -4.35, -5.18, -6.18, -7.36, -8.75, -10.36, -12.12, -13.72,
                            -14.43]
        self.generate(sample_rate, window_size)

    def _get_weight(self, freq):
        f = np.searchsorted(self.freq_map, freq)
        cmessw = 0
        if f > 0 and f < len(self.msg_weights):
            cmessw = np.interp(freq, self.freq_map, self.msg_weights)
        return cmessw

    def generate(self, sample_rate, window_size):
        """ generates a lookup table of size window_size """
        self.weights = np.zeros(window_size)
        div = sample_rate / self.max_freq
        freq_step = sample_rate / window_size / div
        for i in range(window_size):
            w = self._get_weight(i * freq_step)
            if w != 0:
                w = math.pow(10, w / 20)
            self.weights[i] = w * w

    def get_weight(self, bin):
        """ lookup the weight for given bin number out of window_size bins """
        return self.weights[bin]


if __name__ == "__main__":
    # this is so we can compare output against TestVAD app from Ivan.
    vad = VoiceActivityDetector(16000, 256)
    for x in range(256):
        w = vad.cmw.get_weight(x)
        print(w)
