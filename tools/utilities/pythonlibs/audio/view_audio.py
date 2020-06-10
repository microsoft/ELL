#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     view_audio.py
#  Authors:  Chris Lovett, Chuck Jacobs
#
#  Requires: Python 3.x, numpy, tkinter, matplotlib
#
###################################################################################################

import argparse
import json
import os
from threading import Thread, Lock, get_ident
import sys

import tkinter as tk
from tkinter import BOTH, RIGHT, TOP, X, END
from tkinter import Text
from tkinter.ttk import Frame, LabelFrame, Button, Entry

import numpy as np
import matplotlib
# Embedding matplotlib plots in tkinter views requires using the "TkAgg" backend
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as pyplot
import matplotlib.animation as animation

# local modules
import classifier
import featurizer
import microphone
import speaker
import wav_reader
import vad

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(script_dir, "training")]
import make_vad


class SpectrogramImage(Frame):
    """ A tkinter scrolling spectrogram widget """

    def __init__(self, master, colormap_name="inferno"):
        self.colormap_name = colormap_name
        super(SpectrogramImage, self).__init__(master)
        self.features_figure = Figure(figsize=(5, 4), dpi=100)
        self.subplot = self.features_figure.add_subplot(111)
        self.data_shape = None
        canvas = FigureCanvasTkAgg(self.features_figure, master=self)
        canvas.draw()
        canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=True)

    def begin_animation(self, func):
        # (30 fps is usually fine)
        return animation.FuncAnimation(self.features_figure, func, interval=33, blit=True)

    def clear(self, data):
        self.subplot.clear()
        self.spectrogram_image = self.subplot.imshow(data, vmin=0,
                                                     vmax=1, origin="lower", animated=True,
                                                     cmap=pyplot.get_cmap(self.colormap_name))

    def show(self, data):
        """ the result of this function is an image object that is animatable """
        if self.data_shape != data.shape or self.spectrogram_image is None:
            self.clear(data)
        else:
            min_value = np.min(data)
            max_value = np.max(data)
            if not np.isfinite(min_value):
                min_value = 0
            if not np.isfinite(max_value):
                max_value = 1
            eps = 0.1
            if max_value - min_value < eps:
                max_value = min_value + eps
            self.spectrogram_image.set_clim(min_value, max_value)
            self.spectrogram_image.set_data(data)

        self.data_shape = data.shape
        return self.spectrogram_image


class AudioDemo(Frame):
    """ A demo application class that provides simple GUI for testing featurizer+classifier on
    microphone or wav file input. """

    def __init__(self, featurizer_model=None, classifier_model=None, auto_scale=True,
                 sample_rate=None, channels=None, input_device=None, categories=None,
                 image_width=80, threshold=None, wav_file=None, clear=5, serial=None, vad_model=None,
                 smoothing=None, ignore_list=None):
        """ Initialize AudioDemo object
        featurizer_model - the path to the ELL featurizer
        classifier_model - the path to the ELL classifier
        auto_scale - auto scale audio input to range [-1, 1]
        sample_rate - sample rate to featurizer is expecting
        channels - number of channels featurizer is expecting
        input_device - optional id of microphone to use
        categories - path to file containing category labels
        image_width - width of the spectrogram image
        threshold - ignore predictions that have confidence below this number (e.g. 0.5)
        wav_file - optional wav_file to use  when you click Play
        serial - optional serial input, reading numbers from the given serial port.
        vad_model - optional ELL model containing VoiceActivityDetector
        smoothing - controls the size of the smoothing window (defaults to 0).
        ignore_list - list of category labels to ignore (like 'background' or 'silence')
        """
        super().__init__()

        self.CLASSIFIER_MODEL_KEY = "classifier_model"
        self.FEATURIZER_MODEL_KEY = "featurizer_model"
        self.WAV_FILE_KEY = "wav_file"
        self.CATEGORY_FILE_KEY = "categories"

        self.get_settings_file_name()
        self.load_settings()
        self.reading_input = False
        self.featurizer_model = None
        self.serial_port = serial
        self.smoothing = smoothing
        self.ignore_list = ignore_list

        if featurizer_model:
            self.featurizer_model = featurizer_model
            self.settings[self.FEATURIZER_MODEL_KEY] = featurizer_model
        elif self.FEATURIZER_MODEL_KEY in self.settings:
            self.featurizer_model = self.settings[self.FEATURIZER_MODEL_KEY]

        self.classifier_model = None
        if classifier_model:
            self.classifier_model = classifier_model
            self.settings[self.CLASSIFIER_MODEL_KEY] = classifier_model
        elif self.CLASSIFIER_MODEL_KEY in self.settings:
            self.classifier_model = self.settings[self.CLASSIFIER_MODEL_KEY]

        self.vad = None
        if vad_model:
            self.vad = vad.VoiceActivityDetector(vad_model)
            self.previous_vad = 0

        self.wav_filename = wav_file
        if self.wav_filename is None and self.WAV_FILE_KEY in self.settings:
            self.wav_filename = self.settings[self.WAV_FILE_KEY]

        self.wav_file_list = None
        self.auto_scale = auto_scale
        self.sample_rate = sample_rate if sample_rate is not None else 16000
        self.channels = channels if channels is not None else 1
        self.input_device = input_device
        self.num_classifier_features = None

        self.vad = None
        self.vad_reset = (vad_model is not None)
        self.previous_vad = 0
        self.vad_latch = 3  # only reset after 3 vad=0 signals to smooth vad signal a bit.
        if not categories and self.CATEGORY_FILE_KEY in self.settings:
            categories = self.settings[self.CATEGORY_FILE_KEY]

        self.categories = categories
        if categories:
            self.settings[self.CATEGORY_FILE_KEY] = categories

        self.save_settings()  # in case we just changed it.
        self.audio_level = 0
        self.min_level = 0
        self.max_level = 0
        self.threshold = threshold

        self.output_clear_time = int(clear * 1000) if clear else 5000

        self.featurizer = None
        self.classifier = None
        self.wav_file = None
        self.speaker = None
        self.microphone = None
        self.animation = None
        self.show_classifier_output = True
        self.last_prediction = None
        self.probability = 0

        # Threads
        self.read_input_thread = None
        self.lock = Lock()
        self.main_thread = get_ident()
        self.message_queue = []

        # UI components
        self.max_spectrogram_width = image_width
        self.features_entry = None
        self.classifier_feature_data = None
        self.spectrogram_image_data = None

        self.init_ui()

        if self.featurizer_model:
            self.load_featurizer_model(os.path.abspath(self.featurizer_model))
        else:
            self.show_output("Please specify and load a feature model")

        if smoothing == "vad":
            # smooth up to 1 second worth of predictions
            self.smoothing = int(self.sample_rate / self.featurizer.input_size)
            if vad_model is None:
                vad_model = make_vad.make_vad("vad.ell", self.sample_rate, self.featurizer.input_size,
                                              self.featurizer.output_size, None)

        if self.classifier_model:
            self.load_classifier(self.classifier_model)
            self.setup_spectrogram_image()
        else:
            self.show_output("Please specify and load a classifier model")

        if vad_model:
            self.vad = vad.VoiceActivityDetector(vad_model)

    def get_settings_file_name(self):
        """ this app stores the various UI field values in a settings file in your temp folder
        so you don't always have to specify the full command line options """
        import tempfile
        temp = tempfile.gettempdir()
        self.settings_file_name = os.path.join(temp, "ELL", "Audio", "viewaudio.json")

    def load_settings(self):
        """ load the previously saved settings from disk, if any """
        self.settings = {}
        print("loading settings from: {}".format(self.settings_file_name))
        if os.path.isfile(self.settings_file_name):
            with open(self.settings_file_name, "r") as f:
                self.settings = json.load(f)

    def save_settings(self):
        """ save the current settings to disk """
        settings_dir = os.path.dirname(self.settings_file_name)
        if not os.path.isdir(settings_dir):
            os.makedirs(settings_dir)
        with open(self.settings_file_name, "w") as f:
            json.dump(self.settings, f, indent=2)

    def load_featurizer_model(self, featurizer_model):
        """ load the given compiled ELL featurizer for use in processing subsequent audio input """
        if featurizer_model:
            self.featurizer = featurizer.AudioTransform(featurizer_model, 40)
            self.setup_spectrogram_image()

            self.show_output("Feature input size: {}, output size: {}".format(
                self.featurizer.input_size,
                self.featurizer.output_size))
            if self.features_entry.get() != featurizer_model:
                self.features_entry.delete(0, END)
                self.features_entry.insert(0, featurizer_model)

        self.init_data()

    def load_classifier(self, classifier_path):
        """ load the given compiled ELL classifier for use in processing subsequent audio input """
        if classifier_path:
            self.classifier = classifier.AudioClassifier(classifier_path, self.categories, self.threshold,
                                                         smoothing_window=self.smoothing,
                                                         ignore_list=self.ignore_list)
            self.show_output("Classifier input size: {}, output size: {}".format(
                self.classifier.input_size,
                self.classifier.output_size))
            if self.classifier_entry.get() != classifier_path:
                self.classifier_entry.delete(0, END)
                self.classifier_entry.insert(0, classifier_path)
        self.init_data()

    def init_data(self):
        """ initialize the spectrogram_image_data and classifier_feature_data based on the newly loaded model info """
        if self.featurizer:
            dim = (self.featurizer.output_size, self.max_spectrogram_width)
            self.spectrogram_image_data = np.zeros(dim, dtype=float)
            if self.spectrogram_widget:
                self.spectrogram_widget.clear(self.spectrogram_image_data)

            if self.classifier:
                self.num_classifier_features = self.classifier.input_size // self.featurizer.output_size
                dim = (self.num_classifier_features, self.featurizer.output_size)
                self.classifier_feature_data = np.zeros(dim, dtype=float)

    def accumulate_feature(self, feature_data):
        """ accumulate the feature data and pass feature data to classifier """
        if self.vad:
            vad_signal = self.vad.predict(feature_data)
            if self.previous_vad != vad_signal:
                if vad_signal == 0:
                    if self.vad_latch > 0:
                        # wait for 2 more to smooth the vad signal a bit.
                        self.vad_latch -= 1
                    else:
                        self.vad_latch = 3
                        self.previous_vad = vad_signal

                        if self.vad_reset:
                            self.show_output("--- reset ---")
                            self.classifier.reset()
                        else:
                            self.show_output("--- clear history ---")
                            self.classifier.clear_smoothing()

                elif vad_signal == 1:
                    self.vad_latch = 3
                    self.previous_vad = vad_signal

        self.audio_level = np.sum([x * x for x in feature_data])

        if self.classifier and self.show_classifier_output:
            self.classifier_feature_data = np.vstack((self.classifier_feature_data,
                                                      feature_data))[-self.num_classifier_features:, :]
            self.evaluate_classifier()

    def accumulate_spectrogram_image(self, feature_data):
        """ accumulate the feature data into the spectrogram image """
        image_data = self.spectrogram_image_data
        feature_data = np.reshape(feature_data, [-1, 1])
        new_image = np.hstack((image_data, feature_data))[:, -image_data.shape[1]:]
        image_data[:, :] = new_image

    def update_rgb_led(self):
        # This helper function uses the RGB led UI to give an indication of audio levels (brightness)
        # and voice activity (red)
        level = self.audio_level
        if level < self.min_level:
            self.min_level = level
        if level > self.max_level:
            self.max_level = level

        red = 0.0
        green = 0.0
        blue = 0.0

        range = self.max_level - self.min_level
        if range == 0:
            range = 1.0
        brightness = 128 * (level - self.min_level) / range
        if self.previous_vad:
            red = brightness + 127
        else:
            green = brightness + 127
        rgb = "#{:02x}{:02x}{:02x}".format(int(red), int(green), int(blue))
        self.rgb_canvas.itemconfig(self.rgb_oval, fill=rgb)

    def on_ui_update(self):
        # this is an animation callback to update the UI every 33 milliseconds.
        self.update_rgb_led()
        self.process_output()
        result = self.set_spectrogram_image()
        if not self.reading_input:
            self.after(1, self.on_stopped)
        return (result,)

    def set_spectrogram_image(self):
        """ update the spectrogram image and the min/max values """
        self.lock.acquire()  # protect access to the shared state
        result = self.spectrogram_widget.show(self.spectrogram_image_data)
        self.lock.release()
        return result

    def get_correct_shape(self, shape):
        """ for some reason keras stores input shape as (None,80,40), and numpy hates that
        so we have to change this to (1,80,40) """
        shape = list(shape)
        fix = [x if x else 1 for x in shape]
        return tuple(fix)

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

    def evaluate_classifier(self):
        """ run the classifier model on the current feature data and show the prediction, if any """
        if self.evaluate_classifier and self.classifier and self.classifier_feature_data is not None:
            prediction, probability, label, _ = self.classifier.predict(self.classifier_feature_data.ravel())
            if prediction is not None:
                percent = int(100 * probability)
                if label == "silence":
                    self.classifier.reset()
                elif self.last_prediction != prediction or self.probability < probability:
                    self.last_prediction = prediction
                    self.probability = probability
                    self.show_output(" DETECTED ({}) {}% {}".format(prediction, percent, label))

    def start_playing(self, filename):
        """ Play a wav file, and classify the audio. Note we use a background thread to read the
        wav file and we setup a UI animation function to draw the sliding spectrogram image, this way
        the UI update doesn't interfere with the smoothness of the audio playback """
        if self.speaker is None:
            self.speaker = speaker.Speaker()

        self.stop()
        self.reading_input = False
        self.wav_file = wav_reader.WavReader(self.sample_rate, self.channels, self.auto_scale)
        self.wav_file.open(filename, self.featurizer.input_size, self.speaker)

        def update_func(frame_index):
            return self.on_ui_update()

        if self.animation:
            self.animation.event_source.stop()
        self.reading_input = True

        # Start animation timer for updating the UI (e.g. spectrogram image)
        self.animation = self.spectrogram_widget.begin_animation(update_func)

        # start background thread to read and classify the audio.
        self.featurizer.open(self.wav_file)
        self.read_input_thread = Thread(target=self.on_read_features, args=())
        self.read_input_thread.daemon = True
        self.read_input_thread.start()

    def start_recording(self):
        """ Start recording audio from the microphone nd classify the audio. Note we use a background thread to
        process the audio and we setup a UI animation function to draw the sliding spectrogram image, this way
        the UI update doesn't interfere with the smoothness of the microphone readings """

        self.stop()

        input_channel = None

        if self.serial_port:
            import serial_reader
            self.serial = serial_reader.SerialReader(0.001)
            self.serial.open(self.featurizer.input_size, self.serial_port)
            input_channel = self.serial
        else:
            if self.microphone is None:
                self.microphone = microphone.Microphone(auto_scale=self.auto_scale, console=False)

            num_channels = 1
            self.microphone.open(self.featurizer.input_size, self.sample_rate, num_channels, self.input_device)
            input_channel = self.microphone

        def update_func(frame_index):
            return self.on_ui_update()

        if self.animation:
            self.animation.event_source.stop()

        self.reading_input = True
        # Start animation timer for updating the UI (e.g. spectrogram image)
        self.animation = self.spectrogram_widget.begin_animation(update_func)

        # start background thread to read and classify the recorded audio.
        self.featurizer.open(input_channel)
        self.read_input_thread = Thread(target=self.on_read_features, args=())
        self.read_input_thread.daemon = True
        self.read_input_thread.start()

    def on_read_features(self):
        """ this is the background thread entry point.  So we read the feature data in a loop
        and pass it to the classifier """
        try:
            while self.reading_input and self.featurizer:
                feature_data = self.featurizer.read()
                if feature_data is None:
                    break  # eof
                else:
                    self.lock.acquire()
                    self.accumulate_feature(feature_data)
                    self.accumulate_spectrogram_image(feature_data)
                    self.lock.release()
        except:
            errorType, value, traceback = sys.exc_info()
            print("### Exception reading input: " + str(errorType) + ": " + str(value) + " " + str(traceback))
            while traceback:
                print(traceback.tb_frame.f_code)
                traceback = traceback.tb_next

        self.reading_input = False
        if self.classifier:
            self.classifier.reset()  # good time to reset.

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
        self.last_prediction = None
        self.probability = 0
        if self.classifier:
            self.classifier.reset()  # good time to reset.

    def on_rec_button_click(self):
        """ called when user clicks the record button, same button is used to "stop" recording. """
        if self.rec_button["text"] == "Rec":
            self.rec_button["text"] = "Stop"
            self.play_button["text"] = "Play"
            self.start_recording()
        else:
            self.rec_button["text"] = "Rec"
            self.on_stop()

    def on_play_button_click(self):
        """ called when user clicks the record button, same button is used to "stop" playback """
        if self.play_button["text"] == "Play":
            self.play_button["text"] = "Stop"
            self.rec_button["text"] = "Rec"
            self.on_play()
        else:
            self.play_button["text"] = "Play"
            self.on_stop()

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
        self.stop()

    def get_wav_list(self):
        if self.wav_filename and os.path.isfile(self.wav_filename):
            dir_name = os.path.dirname(self.wav_filename)
            if not self.wav_file_list:
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

    def init_ui(self):
        """ setup the GUI for the app """
        self.master.title("Test")
        self.pack(fill=BOTH, expand=True)

        # Input section
        input_frame = LabelFrame(self, text="Input")
        input_frame.bind("-", self.on_minus_key)
        input_frame.bind("+", self.on_plus_key)
        input_frame.pack(fill=X)
        self.play_button = Button(input_frame, text="Play", command=self.on_play_button_click)
        self.play_button.pack(side=RIGHT, padx=4)

        self.rgb_canvas = tk.Canvas(input_frame, width=20, height=20, bd=0)
        self.rgb_oval = self.rgb_canvas.create_oval(2, 2, 20, 20, fill='#FF0000', width=0)
        self.rgb_canvas.pack(side=RIGHT, padx=4)

        self.rec_button = Button(input_frame, text="Rec", command=self.on_rec_button_click)
        self.rec_button.pack(side=RIGHT, padx=4)

        self.wav_filename_entry = Entry(input_frame, width=24)
        self.wav_filename_entry.pack(fill=X)
        self.wav_filename_entry.delete(0, END)
        if self.wav_filename:
            self.wav_filename_entry.insert(0, self.wav_filename)

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
        if self.featurizer_model:
            self.features_entry.insert(0, self.featurizer_model)

        self.spectrogram_widget = SpectrogramImage(features_frame)
        self.spectrogram_widget.pack(fill=X)

        # Classifier section
        classifier_frame = LabelFrame(self, text="Classifier")
        classifier_frame.pack(fill=X)
        load_classifier_button = Button(classifier_frame, text="Load", command=self.on_load_classifier)
        load_classifier_button.pack(side=RIGHT)
        self.classifier_entry = Entry(classifier_frame, width=8)
        self.classifier_entry.pack(fill=X)
        self.classifier_entry.delete(0, END)
        if self.classifier_model:
            self.classifier_entry.insert(0, self.classifier_model)

        # Output section
        output_frame = LabelFrame(self, text="Output")
        output_frame.pack(fill=BOTH, expand=True)

        self.output_text = Text(output_frame)
        self.output_text.pack(fill=BOTH, padx=4, expand=True)

    def setup_spectrogram_image(self):
        """ this needs to be called if you load a new feature model, because the featurizer output size might have
        changed. """
        if self.featurizer:
            dim = (self.featurizer.output_size, self.max_spectrogram_width)
            self.spectrogram_image_data = np.zeros(dim, dtype=float)
            self.spectrogram_widget.show(self.spectrogram_image_data)

    def on_load_featurizer_model(self):
        """ called when user clicks the Load button for the feature model """
        filename = self.features_entry.get()
        filename = filename.strip('"')
        self.settings[self.FEATURIZER_MODEL_KEY] = filename
        self.save_settings()
        self.stop()
        self.load_featurizer_model(filename)

    def on_load_classifier(self):
        """ called when user clicks the Load button for the feature model """
        self.classifier_model = self.classifier_entry.get()
        self.settings[self.CLASSIFIER_MODEL_KEY] = self.classifier_model
        self.save_settings()
        self.stop()
        self.load_classifier(self.classifier_model)


def main(featurizer_model=None, classifier=None, auto_scale=True, sample_rate=None, channels=None, input_device=None,
         categories=None, image_width=80, threshold=None, wav_file=None, clear=5, serial=None, vad_model=None,
         smoothing=None, ignore_list=None):
    """ Main function to create root UI and AudioDemo object, then run the main UI loop """
    root = tk.Tk()
    root.geometry("800x800")
    app = AudioDemo(featurizer_model, classifier, auto_scale, sample_rate, channels, input_device, categories,
                    image_width, threshold, wav_file, clear, serial, vad_model, smoothing, ignore_list)
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
    arg_parser.add_argument("--classifier", "-c", help="Compiled ELL model to use for classification",
                            default=None)
    arg_parser.add_argument("--sample_rate", "-s", help="Audio sample rate expected by classifier",
                            default=16000, type=int)
    arg_parser.add_argument("--channels", "-ch", help="Audio channels expected by classifier",
                            default=1, type=int)

    arg_parser.add_argument("--input_device", "-d", help="Index of input device (see --list_devices)",
                            default=1, type=int)
    arg_parser.add_argument("--list_devices", help="List available input devices", action="store_true")

    arg_parser.add_argument("--categories", help="Provide categories file that provide labels for each predicted class")
    arg_parser.add_argument("--wav_file", help="Provide an input wav file to test")
    arg_parser.add_argument("--image_width", help="Provide the display width of spectrogram image",
                            type=int, default=80)
    arg_parser.add_argument("--threshold", help="Ignore predictions below given confidence threshold (0 to 1)",
                            type=float, default=0)
    arg_parser.add_argument("--clear", help="Seconds before clearing output (default 5)",
                            type=float, default=5)
    arg_parser.add_argument("--serial", help="Name of serial port to read (default None)")
    arg_parser.add_argument("--auto_scale", help="Whether to auto scale audio input to range [-1, 1]",
                            action="store_true")
    arg_parser.add_argument("--vad", help="Use given vad.ell model to determine when to reset the classifier")
    arg_parser.add_argument("--smoothing", help="Use a smoothing buffer over preditions specifying 'vad' to smooth "
                            "based on VAD signal, or with a fixed number of previous N predictions (default None)")
    arg_parser.add_argument("--ignore_list",
                            help="comma separated list of category labels to ignore (like 'background' or 'silence')")
    args = arg_parser.parse_args()

    if args.serial and args.input_device:
        raise Exception("The --serial and --input_device options are mutually exclusive")

    if args.list_devices:
        microphone.list_devices()
    else:
        main(args.featurizer, args.classifier, args.auto_scale,
             args.sample_rate, args.channels, args.input_device, args.categories,
             args.image_width, args.threshold, args.wav_file, args.clear, args.serial, args.vad, args.smoothing,
             args.ignore_list)
