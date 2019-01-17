#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     vad_test.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x,  numpy, tkinter, matplotlib
#
###################################################################################################

import argparse
import json
import os
import sys
from threading import Thread, Lock, get_ident

import tkinter as tk
from tkinter import BOTH, RIGHT, TOP, X, END
from tkinter import Text
from tkinter.ttk import Frame, LabelFrame, Button, Label, Entry

import numpy as np
import matplotlib
# Embedding matplotlib plots in tkinter views requires using the "TkAgg" backend
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as pyplot
import matplotlib.animation as animation

import featurizer
import wav_reader
import microphone
import vad


class VadTest(Frame):
    """ A demo class that provides simple GUI for testing voice activity detection on microphone or wav file input. """

    def __init__(self, featurizer_path, input_device, wav_file, sample_rate):
        """ Initialize the VadTest object:
        featurizer_path - path to the ELL featurizer to use
        input_device - id of the microphone to use
        wav_file - optional wav_file to use when you click play
        sample_rate - the sample rate to resample the incoming audio
        """
        super().__init__()

        self.FEATURIZER_PATH_KEY = "featurizer_path"
        self.WAV_FILE_KEY = "wav_file"
        self.main_thread = get_ident()
        self.output_clear_time = 5000
        self.channels = 1
        self.init_ui()

        self.get_settings_file_name()
        self.load_settings()

        self.max_spectrogram_width = 120
        self.spectrogram_image = None
        self.spectrogram_image_data = None
        self.show_spectrogram = True
        self.colormap_name = "inferno"
        self.min_value = 0.0
        self.max_value = 1.0
        self.update_minmax = True

        self.levels = []
        self.signals = []
        self.featurizer_path = None
        self.featurizer = None
        self.reading_input = False

        # Threads
        self.read_input_thread = None
        self.lock = Lock()
        self.main_thread = get_ident()
        self.message_queue = []
        self.animation = None

        # featurizer
        if featurizer_path:
            self.featurizer_path = featurizer_path
            self.settings[self.FEATURIZER_PATH_KEY] = featurizer_path
        elif self.FEATURIZER_PATH_KEY in self.settings:
            self.featurizer_path = self.settings[self.FEATURIZER_PATH_KEY]
        self.sample_rate = sample_rate

        self.input_device = input_device
        self.wav_filename = None
        self.wav_file = None
        if wav_file:
            self.wav_filename = wav_file
            self.settings[self.WAV_FILE_KEY] = wav_file
        if self.wav_filename is None and self.WAV_FILE_KEY in self.settings:
            self.wav_filename = self.settings[self.WAV_FILE_KEY]

        self.wav_file_list = None
        self.speaker = None
        self.microphone = None
        self.save_settings()  # in case we just changed it.

        if self.featurizer_path:
            self.load_featurizer_model(os.path.abspath(self.featurizer_path))
        else:
            self.show_output("Please specify and load a feature model")

        self.update_ui()

    def init_ui(self):
        self.master.title("VAD Test")
        self.pack(side="top", fill=BOTH, expand=True)

        # VAD Controls section for controlling these VAD settings:
        controls_frame = LabelFrame(self, text="Controls", height=30)
        Label(controls_frame, text="tau_up:").grid(row=0, column=0)
        self.tau_up = Entry(controls_frame, width=15)
        self.tau_up.grid(row=1, column=0)

        Label(controls_frame, text="tau_down:").grid(row=0, column=1)
        self.tau_down = Entry(controls_frame, width=15)
        self.tau_down.grid(row=1, column=1)

        Label(controls_frame, text="threshold_up:").grid(row=0, column=2)
        self.threshold_up = Entry(controls_frame, width=15)
        self.threshold_up.grid(row=1, column=2)

        Label(controls_frame, text="threshold_down:").grid(row=0, column=3)
        self.threshold_down = Entry(controls_frame, width=15)
        self.threshold_down.grid(row=1, column=3)

        Label(controls_frame, text="large_input:").grid(row=0, column=4)
        self.large_input = Entry(controls_frame, width=15)
        self.large_input.grid(row=1, column=4)

        Label(controls_frame, text="gain_att:").grid(row=0, column=5)
        self.gain_att = Entry(controls_frame, width=15)
        self.gain_att.grid(row=1, column=5)

        Label(controls_frame, text="level_threshold:").grid(row=0, column=6)
        self.level_threshold = Entry(controls_frame, width=15)
        self.level_threshold.grid(row=1, column=6)
        controls_frame.pack(side=TOP)

        # Input section
        input_frame = LabelFrame(self, text="Input")
        input_frame.bind("-", self.on_minus_key)
        input_frame.bind("+", self.on_plus_key)
        input_frame.pack(fill=X)
        self.play_button = Button(input_frame, text="Play", command=self.on_play_button_click)
        self.play_button.pack(side=RIGHT, padx=4)
        self.rec_button = Button(input_frame, text="Rec", command=self.on_rec_button_click)
        self.rec_button.pack(side=RIGHT, padx=4)
        self.wav_filename_entry = Entry(input_frame, width=24)
        self.wav_filename_entry.pack(fill=X)
        self.wav_filename_entry.delete(0, END)

        # Feature section
        features_frame = LabelFrame(self, text="Features")
        features_frame.pack(fill=X)

        features_control_frame = Frame(features_frame)
        features_control_frame.pack(fill=X)
        load_features_button = Button(features_control_frame, text="Load", command=self.on_load_featurizer_model)
        load_features_button.pack(side=RIGHT)
        self.features_entry = Entry(features_control_frame, width=8)
        self.features_entry.pack(fill=X)
        self.features_entry.delete(0, END)

        viz_frame = Frame(features_frame)
        viz_frame.bind("%w", self.on_resized)
        viz_frame.pack(fill=X)
        self.features_figure = Figure(figsize=(5, 4), dpi=96)
        self.subplot = self.features_figure.add_subplot(211)

        self.subplot2 = self.features_figure.add_subplot(212)

        self.canvas = FigureCanvasTkAgg(self.features_figure, master=viz_frame)
        self.canvas.draw()
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=True)

        # Output section
        output_frame = LabelFrame(self, text="Output")
        output_frame.pack(fill=BOTH, expand=True)
        self.bind("<Configure>", self.on_resized)

        self.output_text = Text(output_frame)
        self.output_text.pack(fill=BOTH, padx=4, expand=True)

    def on_resized(self, event):
        window_size = event.width
        box = self.spectrogram_image.get_window_extent()
        scale = (box.x1 - box.x0) / self.max_spectrogram_width
        self.max_spectrogram_width = int((window_size * 0.8) / scale)
        self.setup_spectrogram_image()

    def load_featurizer_model(self, featurizer_path):
        """ load the given compiled ELL featurizer for use in processing subsequent audio input """
        if featurizer_path:
            self.featurizer = featurizer.AudioTransform(featurizer_path, 40)
            self.setup_spectrogram_image()

            self.vad = vad.VoiceActivityDetector(self.sample_rate, self.featurizer.output_size)

            self.show_output("Feature input size: {}, output size: {}".format(
                self.featurizer.input_size,
                self.featurizer.output_size))

        self.init_data()

    def setup_spectrogram_image(self):
        """ this need to be called if you load a new feature model, because the featurizer output size might have
        changed. """
        if self.featurizer:
            dim = (self.featurizer.output_size, self.max_spectrogram_width)
            self.spectrogram_image_data = np.zeros(dim, dtype=float)
            self.subplot.clear()
            self.spectrogram_image = self.subplot.imshow(self.spectrogram_image_data, vmin=self.min_value,
                                                         vmax=self.max_value, origin="lower", animated=True,
                                                         cmap=pyplot.get_cmap(self.colormap_name))

    def accumulate_spectrogram_image(self, feature_data):
        """ accumulate the feature data into the spectrogram image """
        image_data = self.spectrogram_image_data
        feature_data = np.reshape(feature_data, [-1, 1])
        new_image = np.hstack((image_data, feature_data))[:, -image_data.shape[1]:]
        image_data[:, :] = new_image

    def set_spectrogram_image(self):
        """ update the spectrogram image and the min/max values """
        self.lock.acquire()  # protect access to the shared state
        if self.update_minmax and self.show_spectrogram:
            min_value = np.min(self.spectrogram_image_data)
            max_value = np.max(self.spectrogram_image_data)
            if np.isfinite(min_value) and np.isfinite(max_value):
                self.min_value = min_value
                self.max_value = max_value
                eps = 0.1
                if self.max_value - self.min_value < eps:
                    self.max_value = self.min_value + eps

            self.spectrogram_image.set_clim(self.min_value, self.max_value)
        self.spectrogram_image.set_data(self.spectrogram_image_data)
        self.lock.release()

    def on_load_featurizer_model(self):
        """ called when user clicks the Load button for the feature model """
        filename = self.features_entry.get()
        filename = filename.strip('"')
        self.featurizer_path = filename
        self.get_sample_rate()
        self.settings[self.FEATURIZER_PATH_KEY] = filename
        self.save_settings()
        self.stop()
        self.load_featurizer_model(filename)

    def set_entry(self, e, value):
        s = str(value)
        if e.get() != s:
            e.delete(0, END)
            e.insert(0, s)

    def get_entry(self, e):
        v = e.get()
        return float(v)

    def update_ui(self):
        self.set_entry(self.wav_filename_entry, self.wav_filename)
        self.set_entry(self.features_entry, self.featurizer_path)
        self.set_entry(self.tau_up, vad.DEFAULT_TAU_UP)
        self.set_entry(self.tau_down, vad.DEFAULT_TAU_DOWN)
        self.set_entry(self.threshold_up, vad.DEFAULT_THRESHOLD_UP)
        self.set_entry(self.threshold_down, vad.DEFAULT_THRESHOLD_DOWN)
        self.set_entry(self.large_input, vad.DEFAULT_LARGE_INPUT)
        self.set_entry(self.gain_att, vad.DEFAULT_GAIN_ATT)
        self.set_entry(self.level_threshold, vad.DEFAULT_LEVEL_THRESHOLD)

    def read_ui_settings(self):
        self.vad.configure(
            self.get_entry(self.tau_up),
            self.get_entry(self.tau_down),
            self.get_entry(self.threshold_up),
            self.get_entry(self.threshold_down),
            self.get_entry(self.large_input),
            self.get_entry(self.gain_att),
            self.get_entry(self.level_threshold)
        )

    def init_data(self):
        """ initialize the spectrogram_image_data based on the newly loaded model info """
        if self.featurizer:
            dim = (self.featurizer.output_size, self.max_spectrogram_width)
            self.spectrogram_image_data = np.zeros(dim, dtype=float)
            if self.spectrogram_image is not None:
                self.spectrogram_image.set_data(self.spectrogram_image_data)

    def get_settings_file_name(self):
        """ this app stores the various UI field values in a settings file in your temp folder
        so you don't always have to specify the full command line options """
        import tempfile
        temp = tempfile.gettempdir()
        self.settings_file_name = os.path.join(temp, "ELL", "Audio", "vad_test.json")

    def load_settings(self):
        """ load the previously saved settings from disk, if any """
        self.settings = {}
        try:
            if os.path.isfile(self.settings_file_name):
                with open(self.settings_file_name, "r") as f:
                    self.settings = json.load(f)
        except:
            self.show_output("error loading settings: {}".format(self.settings_file_name))
            self.settings = {}

    def save_settings(self):
        """ save the current settings to disk """
        settings_dir = os.path.dirname(self.settings_file_name)
        if not os.path.isdir(settings_dir):
            os.makedirs(settings_dir)
        with open(self.settings_file_name, "w") as f:
            f.write(json.dumps(self.settings))

    def on_rec_button_click(self):
        """ called when user clicks the record button, same button is used to "stop" recording. """
        if self.rec_button["text"] == "Rec":
            self.rec_button["text"] = "Stop"
            self.play_button["text"] = "Play"
            self.start_recording()
        else:
            self.rec_button["text"] = "Rec"
            self.on_stopped()

    def on_play_button_click(self):
        """ called when user clicks the record button, same button is used to "stop" playback """
        if self.play_button["text"] == "Play":
            self.play_button["text"] = "Stop"
            self.rec_button["text"] = "Rec"
            self.on_play()
        else:
            self.play_button["text"] = "Play"
            self.on_stopped()

    def on_play(self):
        """ called when user clicks the Play button """
        filename = self.wav_filename_entry.get()
        filename = filename.strip('"')
        self.wav_filename = filename
        self.settings[self.WAV_FILE_KEY] = filename
        self.save_settings()
        self.start_playing(filename)

    def on_stop(self):
        """ called when user clicks the Stop button """
        self.reading_input = False
        if self.wav_file:
            self.wav_file.close()
            self.wav_file = None
        if self.read_input_thread:
            self.read_input_thread.join()
            self.read_input_thread = None
        self.stop()

    def on_stopped(self):
        """ called when we reach the end of the wav file playback """
        self.play_button["text"] = "Play"
        self.on_stop()
        self.subplot2.clear()
        if (len(self.levels) > 0):
            levels = np.array(self.levels)
            levels /= np.max(levels)
            signals = np.array(self.signals)
            self.subplot2.plot(levels)
            self.subplot2.plot(signals)
            self.vad.reset()
        self.canvas.draw()
        self.canvas.show()
        self.levels = []
        self.signals = []

    def stop(self):
        """ called when user clicks the stop button, or we reach the end of a wav file input """
        # close streams
        if self.animation:
            self.animation.event_source.stop()
            self.animation = None
        if self.microphone:
            self.microphone.close()
        if self.speaker:
            self.speaker.close()
        if self.wav_file:
            self.wav_file.close()
            self.wav_file = None
        self.reading_input = False

    def get_wav_list(self):
        if self.wav_filename and os.path.isfile(self.wav_filename):
            full_path = os.path.abspath(self.wav_filename)
            dir_name = os.path.dirname(full_path)
            if not self.wav_file_list:
                print("wav file name: {}".format(full_path))
                print("looking for wav files in: {}".format(dir_name))
                self.wav_file_list = [x for x in os.listdir(dir_name) if os.path.splitext(x)[1] == ".wav"]
                self.wav_file_list.sort()
        return self.wav_file_list

    def select_wav_file(self, filename):
        self.wav_filename = filename
        # show the file in the UI
        self.wav_filename_entry.delete(0, END)
        if self.wav_filename:
            self.wav_filename_entry.insert(0, self.wav_filename)
        # and automatically play the file.
        self.on_play()

    def on_minus_key(self, event):
        """ When user presses the plus button we reverse to the previous wav file in the current folder.
        This way you can easily step through all the training wav files """
        if self.get_wav_list():
            i = self.wav_file_list.index(os.path.basename(self.wav_filename))
            if i - 1 >= 0:
                next_wav_file = self.wav_file_list[i - 1]
                dir_name = os.path.dirname(self.wav_filename)
                self.select_wav_file(os.path.join(dir_name, next_wav_file))

    def on_plus_key(self, event):
        """ When user presses the plus button we advance to the next wav file in the current folder.
        This way you can easily step through all the training wav files """
        if self.get_wav_list():
            i = self.wav_file_list.index(os.path.basename(self.wav_filename))
            if i + 1 < len(self.wav_file_list):
                next_wav_file = self.wav_file_list[i + 1]
                dir_name = os.path.dirname(self.wav_filename)
                self.select_wav_file(os.path.join(dir_name, next_wav_file))

    def clear_output(self):
        """ remove some of the Output based a the timeout callback  """
        self.output_text.delete(1.0, 2.0)

    def process_output(self):
        """ show output that was queued by background thread """
        self.lock.acquire()
        messages = self.message_queue
        self.message_queue = []
        self.lock.release()
        for msg in messages:
            self.show_output(msg)

    def show_output(self, message):
        """ show output message, or queue it if we are on a background thread """
        if self.main_thread != get_ident():
            self.message_queue += [message]
            return

        for line in str(message).split('\n'):
            self.output_text.insert(END, "{}\n".format(line))

        self.output_text.see("end")  # scroll to end
        self.after(self.output_clear_time, self.clear_output)

    def start_playing(self, filename):
        """
        Play a wav file, and classify the audio. Note we use a background thread to read the
        wav file and we setup a UI animation function to draw the sliding spectrogram image, this way
        the UI update doesn't interfere with the smoothness of the audio playback
        """

        self.stop()

        self.read_ui_settings()
        self.reading_input = False
        self.wav_file = wav_reader.WavReader(self.sample_rate, self.channels)
        self.wav_file.open(filename, self.featurizer.input_size, self.speaker)
        self.setup_spectrogram_image()

        def update_func(frame_index):
            self.process_output()
            if not self.reading_input:
                self.after(1, self.on_stopped)
            self.set_spectrogram_image()
            return (self.spectrogram_image,)

        if self.animation:
            self.animation.event_source.stop()
        self.reading_input = True

        # Start animation timer for updating the UI (e.g. spectrogram image) (30 fps is usually fine)
        self.animation = animation.FuncAnimation(self.features_figure, update_func, interval=33, blit=True)

        # start background thread to read and classify the audio.
        self.featurizer.open(self.wav_file)
        self.read_input_thread = Thread(target=self.on_read_features, args=())
        self.read_input_thread.daemon = True
        self.read_input_thread.start()

    def start_recording(self):
        """ Start recording audio from the microphone nd classify the audio. Note we use a background thread to
        process the audio and we setup a UI animation function to draw the sliding spectrogram image, this way
        the UI update doesn't interfere with the smoothness of the microphone readings """
        if self.microphone is None:
            self.microphone = microphone.Microphone(False)

        self.stop()
        self.read_ui_settings()
        num_channels = 1
        self.microphone.open(self.featurizer.input_size, self.sample_rate, num_channels, self.input_device)

        def update_func(frame_index):
            # this is an animation callback to update the UI every 33 milliseconds.
            self.process_output()
            self.set_spectrogram_image()
            if not self.reading_input:
                self.after(1, self.on_stopped)
            return (self.spectrogram_image,)

        if self.animation:
            self.animation.event_source.stop()

        self.reading_input = True
        # Start animation timer for updating the UI (e.g. spectrogram image) (30 fps is usually fine)
        self.animation = animation.FuncAnimation(self.features_figure, update_func, interval=33, blit=True)

        # start background thread to read and classify the recorded audio.
        self.featurizer.open(self.microphone)
        self.read_input_thread = Thread(target=self.on_read_features, args=())
        self.read_input_thread.daemon = True
        self.read_input_thread.start()

    def on_read_features(self):
        """ this is the background thread entry point.  So we read the feature data in a loop """
        try:

            while self.reading_input and self.featurizer:
                feature_data = self.featurizer.read()
                if feature_data is None:
                    break  # eof
                else:
                    signal = self.vad.process(feature_data)
                    self.levels += [self.vad.level]
                    self.signals += [signal]
                    self.lock.acquire()
                    if self.show_spectrogram:
                        self.accumulate_spectrogram_image(feature_data)
                    self.lock.release()
        except:
            errorType, value, traceback = sys.exc_info()
            print("### Exception reading input: " + str(errorType) + ": " + str(value) + " " + str(traceback))
            while traceback:
                print(traceback.tb_frame.f_code)
                traceback = traceback.tb_next

        self.reading_input = False


def main(featurizer, input_device, wav_file, sample_rate):
    """ Main function to create root UI and AudioDemo object, then run the main UI loop """
    root = tk.Tk()
    root.geometry("800x800")
    app = VadTest(featurizer, input_device, wav_file, sample_rate)
    root.bind("+", app.on_plus_key)
    root.bind("-", app.on_minus_key)
    while True:
        try:
            root.mainloop()
            break
        except UnicodeDecodeError:
            pass


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Test a feature model and optional classifier in a handy GUI app")

    # options
    arg_parser.add_argument("--featurizer", "-m", help="Compiled ELL model to use for generating features",
                            default=None)
    arg_parser.add_argument("--input_device", "-d", help="Index of input device (see --list_devices)",
                            default=1, type=int)
    arg_parser.add_argument("--list_devices", help="List available input devices", action="store_true")
    arg_parser.add_argument("--wav_file", help="Provide an input wav file to test", default=None)
    arg_parser.add_argument("--sample_rate", type=int, help="The sample rate that featurizer is setup to use",
                            default=16000)
    args = arg_parser.parse_args()
    if args.list_devices:
        microphone.list_devices()
    else:
        main(args.featurizer, args.input_device, args.wav_file, args.sample_rate)
