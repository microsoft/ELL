#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     view_audio.py
##  Authors:  Chris Lovett, Chuck Jacobs
##
##  Requires: Python 3.x
##
###################################################################################################

import argparse
import json
import math
import os
import time
from threading import Thread, Lock, get_ident
import sys
import wave

import tkinter as tk
from tkinter import BOTH, LEFT, RIGHT, TOP, BOTTOM, RAISED, X, N, END
from tkinter import Text
from tkinter.ttk import Frame, LabelFrame, Button, Style, Label, Entry

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

class AudioDemo(Frame):
    """ A demo application class that provides simple GUI for testing featurizer+classifier on 
    microphone or wav file input. """

    def __init__(self, feature_model=None, classifier_model=None, 
                sample_rate=None, channels=None, input_device=None, categories=None, 
                image_width=80, ignore_label=None, threshold=None, wav_file=None, clear=5):
        """ Initialize AudioDemo object
        feature_model - the path to the compiled ELL featurizer
        classifier_model - the path to the compiled ELL classifier
        sample_rate - sample rate to featurizer is expecting
        channels - number of channels featurizer is expecting
        input_device - optional id of microphone to use
        categories - path to file containing category labels
        image_width - width of the spectrogram image
        ignore_label - list of predictions to ignore (e.g. [0] ignores prediction 0)
        threshold - ignore predictions that have confidence below this number (e.g. 0.5)
        wav_file - optional wav_file to process when you click Play
        """
        input_device
        super().__init__()

        self.CLASSIFIER_MODEL_KEY = "classifier_model"
        self.FEATURE_MODEL_KEY = "feature_model"
        self.WAV_FILE_KEY = "wav_file"
        self.CATEGORY_FILE_KEY = "categories"

        self.get_settings_file_name()
        self.load_settings()
        self.reading_input = False
        self.feature_model = None
        if feature_model:
            self.feature_model = feature_model
            self.settings[self.FEATURE_MODEL_KEY] = feature_model
        elif self.FEATURE_MODEL_KEY in self.settings:
            self.feature_model = self.settings[self.FEATURE_MODEL_KEY]        

        self.classifier_model = None
        if classifier_model:
            self.classifier_model = classifier_model
            self.settings[self.CLASSIFIER_MODEL_KEY] = classifier_model
        elif self.CLASSIFIER_MODEL_KEY in self.settings:
            self.classifier_model = self.settings[self.CLASSIFIER_MODEL_KEY]

        self.wav_filename = wav_file
        if self.wav_filename is None and self.WAV_FILE_KEY in self.settings:
            self.wav_filename = self.settings[self.WAV_FILE_KEY]

        self.wav_file_list = None

        self.sample_rate = sample_rate if sample_rate is not None else 16000
        self.channels = channels if channels is not None else 1
        self.input_device = input_device
        self.num_classifier_features = None

        if not categories and self.CATEGORY_FILE_KEY in self.settings:
            categories = self.settings[self.CATEGORY_FILE_KEY]

        self.categories = categories
        if categories:
            self.settings[self.CATEGORY_FILE_KEY] = categories
            
        self.save_settings() # in case we just changed it.
        self.min_value = 0.0
        self.max_value = 1.0
        self.update_minmax = True
        
        self.ignore_list = []
        if ignore_label:
            self.ignore_list = [ ignore_label ]
        self.threshold = threshold

        self.output_clear_time = int(clear * 1000) if clear else 5000

        self.featurizer = None
        self.classifier = None
        self.wav_file = None
        self.speaker = None
        self.microphone = None
        self.animation = None
        self.show_spectrogram = True
        self.colormap_name = "plasma"
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
        self.spectrogram_image = None
        self.classifier_feature_data = None
        self.spectrogram_image_data = None
        
        self.init_ui()
        
        if self.feature_model:
            self.load_feature_model(os.path.abspath(self.feature_model))
        else:
            self.show_output("Please specify and load a feature model")

        if self.classifier_model:   
            self.load_classifier(self.classifier_model)    
            self.setup_spectrogram_image()   
        else:
            self.show_output("Please specify and load a classifier model")

    def get_settings_file_name(self):
        """ this app stores the various UI field values in a settings file in your temp folder 
        so you don't always have to specify the full command line options """
        import tempfile
        temp = tempfile.gettempdir() 
        self.settings_file_name = os.path.join(temp, "ELL", "Audio", "viewaudio.json")

    def load_settings(self):
        """ load the previously saved settings from disk, if any """
        self.settings = {}
        if os.path.isfile(self.settings_file_name):
            with open(self.settings_file_name, "r") as f:
                self.settings = json.load(f)

    def save_settings(self):
        """ save the current settings to disk """
        settings_dir = os.path.dirname(self.settings_file_name)
        if not os.path.isdir(settings_dir):
            os.makedirs(settings_dir)
        with open(self.settings_file_name, "w") as f:
            f.write(json.dumps(self.settings))            

    def load_feature_model(self, feature_model):
        """ load the given compiled ELL featurizer for use in processing subsequent audio input """
        if feature_model:
            self.featurizer = featurizer.AudioTransform(feature_model, 40)
            self.setup_spectrogram_image()
            
            self.show_output("Feature input size: {}, output size: {}".format(
                self.featurizer.input_size, 
                self.featurizer.output_size))
            if self.features_entry.get() != feature_model:
                self.features_entry.delete(0, END)
                self.features_entry.insert(0, feature_model)
            
        self.init_data()

    def load_classifier(self, classifier_path):
        """ load the given compiled ELL classifier for use in processing subsequent audio input """
        if classifier_path:
            self.classifier = classifier.AudioClassifier(classifier_path, self.categories, self.ignore_list, self.threshold)
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
            self.spectrogram_image_data = np.zeros((self.featurizer.output_size, self.max_spectrogram_width), dtype=float)  
            if self.spectrogram_image is not None:
                self.spectrogram_image.set_data(self.spectrogram_image_data)

            if self.classifier:
                self.num_classifier_features = self.classifier.input_size // self.featurizer.output_size
                print("Classifier requires {} buffered features of size {}".format(self.num_classifier_features, self.featurizer.output_size))
                self.classifier_feature_data = np.zeros((self.num_classifier_features, self.featurizer.output_size), dtype=float)

    def accumulate_feature(self, feature_data):
        """ accumulate the feature data and pass feature data to classifier """
        if self.classifier and self.show_classifier_output:
            self.classifier_feature_data = np.vstack((self.classifier_feature_data, feature_data))[-self.num_classifier_features:,:]
            self.evaluate_classifier()
            
    def accumulate_spectrogram_image(self, feature_data):
        """ accumulate the feature data into the spectrogram image """
        image_data = self.spectrogram_image_data
        feature_data = np.reshape(feature_data, [-1,1])
        new_image = np.hstack((image_data, feature_data))[:,-image_data.shape[1]:]
        image_data[:,:] = new_image

    def set_spectrogram_image(self):
        """ update the spectrogram image and the min/max values """
        self.lock.acquire() # protect access to the shared state 
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

        self.output_text.see("end") # scroll to end
        self.after(self.output_clear_time, self.clear_output)

    def evaluate_classifier(self):
        """ run the classifier model on the current feature data and show the prediction, if any """
        if self.evaluate_classifier and self.classifier and self.classifier_feature_data is not None:
            prediction, probability, label = self.classifier.predict(self.classifier_feature_data.ravel())
            if prediction is not None:
                percent = int(100*probability)
                if self.last_prediction != prediction or self.probability < probability:
                    self.last_prediction = prediction
                    self.probability = probability
                    self.show_output("<<< DETECTED ({}) {}% {} >>>".format(prediction, percent, label))
    
    def start_playing(self, filename):
        """ Play a wav file, and classify the audio. Note we use a background thread to read the
        wav file and we setup a UI animation function to draw the sliding spectrogram image, this way
        the UI update doesn't interfere with the smoothness of the audio playback """
        if self.speaker is None:
            self.speaker = speaker.Speaker()
    
        self.stop()
        self.reading_input = False
        self.wav_file = wav_reader.WavReader(self.sample_rate, self.channels)
        self.wav_file.open(filename, self.featurizer.input_size, self.speaker)

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
        """ this is the background thread entry point.  So we read the feature data in a loop
        and pass it to the classifier """
        try:           
            while self.reading_input and self.featurizer:
                feature_data = self.featurizer.read()                
                if feature_data is None:
                    break # eof
                else:                    
                    self.lock.acquire()
                    self.accumulate_feature(feature_data)
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

    def on_plus_key(self, event):
        """ When user presses the plus button we advance to the next wav file in the current folder.
        This way you can easily step through all the training wav files """
        if self.wav_filename and os.path.isfile(self.wav_filename):
            dir_name = os.path.dirname(self.wav_filename)
            if not self.wav_file_list:
                self.wav_file_list = [x for x in os.listdir(dir_name) if os.path.splitext(x)[1] == ".wav"]
                self.wav_file_list.sort()
            try:
                i = self.wav_file_list.index(os.path.basename(self.wav_filename))
                if i + 1 < len(self.wav_file_list):
                    next_wav_file = self.wav_file_list[i+1]
                    self.wav_filename = os.path.join(dir_name, next_wav_file)
                    # show the file in the UI                                
                    self.wav_filename_entry.delete(0, END)
                    if self.wav_filename:
                        self.wav_filename_entry.insert(0, self.wav_filename)
                    # and automatically play the file.
                    self.on_play()
            except:
                pass            

    def init_ui(self):
        """ setup the GUI for the app """
        self.master.title("Test")
        self.pack(fill=BOTH, expand=True)

        # Input section
        input_frame = LabelFrame(self, text="Input")
        input_frame.bind("+", self.on_plus_key)
        input_frame.pack(fill=X)
        self.play_button = Button(input_frame, text="Play", command=self.on_play_button_click)
        self.play_button.pack(side=RIGHT, padx=4)
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
        load_features_button = Button(features_control_frame, text="Load", command=self.on_load_feature_model)
        load_features_button.pack(side=RIGHT)
        self.features_entry = Entry(features_control_frame, width=8)
        self.features_entry.pack(fill=X)
        self.features_entry.delete(0, END)
        if self.feature_model:
            self.features_entry.insert(0, self.feature_model)

        viz_frame = Frame(features_frame)
        viz_frame.pack(fill=X)
        self.features_figure = Figure(figsize=(5, 4), dpi=100)
        self.subplot = self.features_figure.add_subplot(111)

        canvas = FigureCanvasTkAgg(self.features_figure, master=viz_frame)
        canvas.draw()
        canvas.show()
        canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=True)

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
        """ this need to be called if you load a new feature model, because the featurizer output size might have changed. """
        if self.featurizer:
            self.spectrogram_image_data = np.zeros((self.featurizer.output_size, self.max_spectrogram_width), dtype=float)  
            self.subplot.clear()
            self.spectrogram_image = self.subplot.imshow(self.spectrogram_image_data, vmin=self.min_value, vmax=self.max_value, origin="lower", animated=True, cmap=pyplot.get_cmap(self.colormap_name))

    def on_load_feature_model(self):
        """ called when user clicks the Load button for the feature model """
        filename = self.features_entry.get()
        filename = filename.strip('"')
        self.settings[self.FEATURE_MODEL_KEY] = filename
        self.save_settings()
        self.stop()
        self.load_feature_model(filename)

    def on_load_classifier(self):
        """ called when user clicks the Load button for the feature model """
        self.classifier_model = self.classifier_entry.get()
        self.settings[self.CLASSIFIER_MODEL_KEY] = self.classifier_model
        self.save_settings()
        self.stop()
        self.load_classifier(self.classifier_model)

def main(feature_model=None, classifier=None, sample_rate=None, channels=None, input_device=None, categories=None, image_width=80, ignore_label=None, threshold=None, wav_file=None, clear=5):
    """ Main function to create root UI and AudioDemo object, then run the main UI loop """
    root = tk.Tk()
    root.geometry("800x800")
    app = AudioDemo(feature_model, classifier, sample_rate, channels, input_device, categories, image_width, ignore_label, threshold, wav_file, clear)
    root.bind("+", app.on_plus_key)
    while True:
        try:
            root.mainloop()
            break
        except UnicodeDecodeError:
            pass

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Test a feature model and optional classifier in a handy GUI app")

    # options
    arg_parser.add_argument("--feature_model", "-m", help="Compiled ELL model to use for generating features", default=None)
    arg_parser.add_argument("--classifier_model", "-c", help="Compiled ELL model to use for classification", default=None)
    arg_parser.add_argument("--sample_rate", "-s", help="Audio sample rate expected by classifier", default=16000, type=int)
    arg_parser.add_argument("--channels", "-ch", help="Audio channels expected by classifier", default=1, type=int)
    arg_parser.add_argument("--input_device", "-d", help="Input device", default=None, type=int)
    arg_parser.add_argument("--categories", help="Provide categories file that provide labels for each predicted class", default=None)
    arg_parser.add_argument("--wav_file", help="Provide an input wav file to test", default=None)
    arg_parser.add_argument("--image_width", type=int, help="Provide the display width of spectrogram image", default=80)
    arg_parser.add_argument("--ignore_label", type=int, help="Ignore the given label when predicted", default=None)
    arg_parser.add_argument("--threshold", type=float, help="Ignore predictions below given confidence threshold (between 0 and 1)", default=0)
    arg_parser.add_argument("--clear", type=float, help="Seconds before clearing output (default 5)", default=5)
    args = arg_parser.parse_args()

    main(args.feature_model, args.classifier_model, 
        args.sample_rate, args.channels, args.input_device, args.categories, 
        args.image_width, args.ignore_label, args.threshold, args.wav_file, args.clear)

