#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     audio_unittest.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import json
import os
import unittest
import sys
from shutil import rmtree, copyfile

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path))
sys.path.append(os.path.join(script_path, ".."))
sys.path.append(os.path.join(script_path, "..", ".."))
sys.path.append(os.path.join(script_path, "..", "training"))

import find_ell  # noqa: F401
import ell  # noqa: F401
from download_helper import *  # noqa: F403
import logger
import make_featurizer

sys.path.append(os.path.join(script_path, "..", "..", "..", "..", "importers", "onnx"))
import onnx_import

sys.path.append(os.path.join(script_path, "..", "..", "..", "..", "wrap"))
import wrap
import buildtools

_log = logger.init()

example_data = os.path.join(script_path, "..", "..", "..", "..", "..", "examples", "data")


class AudioUnitTest(unittest.TestCase):

    def get_test_model_repo(self):
        return "https://github.com/microsoft/ELL-models/"

    def wrap_model(self, model, target_dir, module_name, language):
        if os.path.isdir(target_dir):
            rmtree(target_dir)
        builder = wrap.ModuleBuilder()
        args = ["--model_file", model,
                "--outdir", os.path.join(target_dir),
                "--language", language,
                "--target", "host",
                "--module_name", module_name]
        builder.parse_command_line(args)
        builder.run()

    def make_project(self, target_dir):

        build_dir = os.path.join(target_dir, "build")
        if os.path.isdir(build_dir):
            rmtree(build_dir)
        os.makedirs(build_dir)

        current_path = os.getcwd()
        os.chdir(build_dir)
        cmd = buildtools.EllBuildTools(find_ell.get_ell_root())
        cmd.cmake_generate("..")

        make = ["make"]
        if os.name == 'nt':
            make = ["cmake", "--build", ".", "--config", "Release"]
        cmd.run(make, print_output=True)
        os.chdir(current_path)

    def make_featurizer(self, model_path, filename):
        folder = os.path.dirname(os.path.abspath(model_path))
        with open(os.path.join(folder, "train_results.json"), "r") as f:
            train_data = json.load(f)
        item = train_data["dataset"]
        self.input_size = item["input_size"]
        self.num_filters = item["num_filters"]
        self.sample_rate = item["sample_rate"]
        self.auto_scale = item["auto_scale"] if "auto_scale" in item else False

        return make_featurizer.make_featurizer(filename, self.sample_rate, self.input_size, self.input_size,
                                               filterbank_type="mel", filterbank_size=self.num_filters, log_node=True,
                                               filterbank_nfft=512, hamming_window=True)

    def compile_model(self, model_file, outputdir, module_name):
        self.wrap_model(model_file, outputdir, module_name, "python")
        self.make_project(outputdir)

    def download_model(self):
        # https://github.com/microsoft/ELL-models/raw/master/models/speech_commands_v0.01/BluePaloVerde/GRU110KeywordSpotter.onnx.zip
        path = self.get_test_model_repo()
        classifier = path + "raw/master/models/speech_commands_v0.01/BluePaloVerde/GRU110KeywordSpotter.onnx.zip"
        categories = path + "raw/master/models/speech_commands_v0.01/BluePaloVerde/categories.txt"
        train_results = path + "raw/master/models/speech_commands_v0.01/BluePaloVerde/train_results.json"
        local_onnx_file = download_and_extract_model(classifier, model_extension=".onnx",  # noqa: F405
                                                     local_folder="test")
        self.train_results_file = download_file(train_results, local_folder="test")  # noqa: F405
        self.categories_file = download_file(categories, local_folder="test")  # noqa: F405
        _log.info("Unzipped: {}".format(local_onnx_file))
        self.classifier_model = onnx_import.convert(local_onnx_file)
        self.featurizer_model = self.make_featurizer(self.classifier_model, "test/featurizer.ell")

    def add_vad_callback(self):
        import model_editor
        editor = model_editor.ModelEditor(self.classifier_model)
        for node in editor.find_rnns():
            editor.add_vad(node, 16000, 512, 1.54, 0.074326, 2.40016, 0.002885, 3.552713, 0.931252, 0.007885)
        editor.attach_sink("VoiceActivityDetector", "VadCallback")
        filename = os.path.splitext(os.path.basename(self.classifier_model))[0]
        self.classifier_model2 = os.path.join(os.path.dirname(self.classifier_model), filename + "2.ell")
        editor.save(self.classifier_model2)

    def run_classifier(self, classifier="test/compiled_classifier/model"):
        try:
            import pyaudio  # noqa: F401
            import run_classifier  # noqa: F401
        except:
            _log.info("---------------- skipping test_keyword_spotter because you don't have pyaudio module.")
            return 0

        wav_dir = os.path.join("test", "wav")
        if not os.path.exists(wav_dir):
            os.mkdir(wav_dir)
        copyfile(os.path.join(example_data, "seven.wav"), os.path.join(wav_dir, "seven.wav"))
        copyfile(os.path.join(example_data, "bed.wav"), os.path.join(wav_dir, "bed.wav"))

        result = run_classifier.test_keyword_spotter("test/compiled_featurizer/mfcc",
                                                     classifier,
                                                     self.categories_file,
                                                     wav_dir,
                                                     threshold=0.80,
                                                     sample_rate=self.sample_rate,
                                                     output_speaker=False,
                                                     auto_scale=self.auto_scale,
                                                     reset=True)
        print(result)
        if len(result) != 2:
            raise Exception("Did not get 2 predictions")

        actual = [x[2] for x in result]

        if actual != ["bed", "seven"]:
            raise Exception("Did not get expected predictions 'bed' and 'seven'")

        return 0

    def test_keyword_spotter(self):
        _log.info("---------------- test_keyword_spotter")
        if not os.path.exists('test'):
            os.mkdir("test")
        self.download_model()
        self.add_vad_callback()
        self.compile_model(self.featurizer_model, "test/compiled_featurizer", "mfcc")
        self.compile_model(self.classifier_model, "test/compiled_classifier", "model")
        self.run_classifier()
        self.compile_model(self.classifier_model2, "test/compiled_classifier2", "model")
        self.run_classifier("test/compiled_classifier2/model")


if __name__ == "__main__":
    unittest.main()
