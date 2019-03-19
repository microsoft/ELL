###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     mixer.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import argparse
import os
import sys

import audioop
import numpy as np

sys.path += [os.path.join(os.path.dirname(__file__), "..")]
import wav_reader
import speaker


class AudioNoiseMixer:
    """ This class provides a way to mix noise in with audio, which is useful during training. """
    def __init__(self, noise_files=None, mix_ratio=0.1, mix_percent=0.2):
        """ 'noise_files' Provides a directory of wav files that will be mixed into the opened
        audio files where the mix_ratio will determine how much of the first audio to mix into
        the second audio.  The mixer will round robin each noise file and will only change mix_percent
        of the audio files given to the open method, leaving the others unchanged """
        self.noise_files = noise_files
        self.mix_ratio = mix_ratio
        self.mix_percent = mix_percent
        self.count = 1
        self.mix = False
        self.noise_index = 0
        self.noise_reader = None
        self.read_size = 0

    def open_noise(self):
        self.mix = True
        self.count = 1
        if self.noise_reader is None:
            buffer_size = self.wav_reader.buffer_size
            self.noise_reader = wav_reader.WavReader(self.requested_rate, self.requested_channels)
            self.noise_reader.open(self.noise_files[self.noise_index], buffer_size)
            self.noise_index += 1
            if self.noise_index == len(self.noise_files):
                self.noise_index = 0

    def open(self, input_wav_reader, speaker=None):
        """ Open the given audio file for mixing.  This mixer will return the same requested #
        channels and sample rate that the wav_reader was given """
        self.wav_reader = input_wav_reader
        self.read_size = self.wav_reader.read_size
        self.dtype = input_wav_reader.dtype
        self.sample_width = input_wav_reader.sample_width
        self.requested_channels = input_wav_reader.requested_channels
        self.requested_rate = input_wav_reader.requested_rate
        self.audio_scale_factor = input_wav_reader.audio_scale_factor
        if float(self.count) * self.mix_percent >= 1:
            self.open_noise()
        else:
            self.count += 1
            self.mix = False

        if speaker:
            # configure output stream to match what we are resampling to...
            audio_format = input_wav_reader.audio.get_format_from_width(self.sample_width)
            speaker.open(audio_format, self.requested_channels, self.requested_rate)
        self.speaker = speaker

    def read(self):
        data = self.wav_reader.read_raw()
        if data is None:
            return None

        # make sure we got an even number of bytes matching the sample_width.
        remainder = len(data) % self.sample_width
        if remainder != 0:
            data = data[:len(data) - remainder]

        if self.noise_reader:
            noise = self.noise_reader.read_raw()
            if noise is None:
                # reached the end of this noise file!
                self.noise_reader.close()
                self.noise_reader = None
                self.open_noise()
            elif len(noise) >= len(data):
                # audioop.add requires same size buffers, so truncate
                # noise if necessary.
                if len(noise) > len(data):
                    noise = noise[:len(data)]

                noise = audioop.mul(noise, 2, self.mix_ratio)
                data = audioop.add(data, noise, 2)

        # Let's hear the combined audio
        if self.speaker:
            self.speaker.write(data)

        # convert to floats.
        data = np.frombuffer(data, dtype=self.dtype).astype(float)

        # pad the last record with zeros so it is valid output.
        data_size = len(data)
        if data_size < self.read_size:
            bigger = np.zeros((self.read_size))
            bigger[:data_size] = data
            data = bigger
        elif data_size > self.read_size:
            data = data[:self.read_size]  # truncate

        return data * self.audio_scale_factor

    def close(self):
        if self.wav_reader1:
            self.wav_reader1.close()
            self.wav_reader1 = None
        if self.wav_reader2:
            self.wav_reader2.close()
            self.wav_reader2 = None

    def is_closed(self):
        return self.wav_reader1 is None


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Test the AudioNoiseMixer class")
    parser.add_argument("--wav_file", "-w", help=".wav file to process")
    parser.add_argument("--noise_dir", "-n", help="directory of .wav files containing noise")
    parser.add_argument("--mix_ratio", "-r", type=float, default=0.1, help="how much noise to add")
    args = parser.parse_args()

    noise_files = []
    noise_dir = args.noise_dir
    for f in os.listdir(noise_dir):
        if os.path.splitext(f)[1] == ".wav":
            noise_files += [os.path.join(noise_dir, f)]

    speaker = speaker.Speaker()
    mixer = AudioNoiseMixer(noise_files, mix_ratio=args.mix_ratio, mix_percent=1)

    reader = wav_reader.WavReader(16000, 1)
    reader.open(args.wav_file, 512)
    mixer.open(reader, speaker)

    while True:
        data = mixer.read()
        if data is None:
            break

    print("finished")
