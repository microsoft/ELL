#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     unittest.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import json
import os
import unittest
import sys
from shutil import rmtree

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path))
sys.path.append(os.path.join(script_path, ".."))
sys.path.append(os.path.join(script_path, "training"))

import find_ell  # noqa: F401
import ell  # noqa: F401
from download_helper import *  # noqa: F403
import logger
import make_featurizer

sys.path.append(os.path.join(script_path, "..", "..", "..", "importers", "onnx"))
import onnx_import

sys.path.append(os.path.join(script_path, "..", "..", "..", "wrap"))
import wrap
import buildtools

_log = logger.init()

example_data = os.path.join(script_path, "..", "..", "..", "..", "examples", "data")


class AudioUnitTest(unittest.TestCase):

    def get_test_model_repo(self):
        config_file = os.path.join(find_ell.find_ell_build(), "config.json")
        if os.path.isfile(config_file):
            with open(config_file, "r") as f:
                config = json.load(f)
                if "test_models_repo" in config:
                    return clone_repo(config["test_models_repo"], get_home_path())  # noqa: F405
            return None
        else:
            raise Exception("Missing config.json file: {}".format(config_file))

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
        cmake = ["cmake", ".."]
        if os.name == 'nt':
            cmake = ["cmake", "-G", "Visual Studio 15 2017 Win64", ".."]
        cmd.run(cmake, print_output=True)

        make = ["make"]
        if os.name == 'nt':
            make = ["cmake", "--build", ".", "--config", "Release"]
        cmd.run(make, print_output=True)
        os.chdir(current_path)

    def make_featurizer(self, model_path, filename):
        with open(os.path.join(model_path, "train_results.json"), "r") as f:
            train_data = json.load(f)
        item = train_data[0]
        self.input_size = item["input_size"]
        self.num_filters = item["num_filters"]
        self.sample_rate = item["sample_rate"]

        return make_featurizer.make_featurizer(filename, self.sample_rate, self.input_size, self.input_size,
                                               filterbank_type="mel", filterbank_size=self.num_filters, log_node=True)

    def compile_model(self, model_file, outputdir, module_name):
        self.wrap_model(model_file, outputdir, module_name, "python")
        self.make_project(outputdir)

    def compile_keyword_spotter(self):
        if not os.path.exists('test'):
            os.mkdir("test")
        path = self.get_test_model_repo()
        path = os.path.join(path, "models", "speech_commands_v0.01")
        if os.path.exists(path):
            _log.info("Found test models at {}".format(path))
        else:
            raise Exception("Audio test models missing from {}".format(path))

        filename = os.path.join(path, "GRU100KeywordSpotter.onnx.zip")
        local_onnx_file = download_and_extract_model(filename, model_extension=".onnx",  # noqa: F405
                                                     local_folder="test")
        self.categories_file = download_file(os.path.join(path, "categories.txt"), local_folder="test")  # noqa: F405
        _log.info("Unzipped: {}".format(local_onnx_file))
        classifier_model = onnx_import.convert(local_onnx_file)
        featurizer_model = self.make_featurizer(path, "test/featurizer.ell")
        self.compile_model(featurizer_model, "test/compiled_featurizer", "mfcc")
        self.compile_model(classifier_model, "test/compiled_classifier", "model")
        self.wav_file = os.path.join(example_data, "seven.wav")

    def test_keyword_spotter(self):
        _log.info("---------------- test_keyword_spotter")
        self.compile_keyword_spotter()

        try:
            import pyaudio  # noqa: F401
            import test_audio  # noqa: F401
        except:
            _log.info("---------------- skipping test_keyword_spotter because you don't have pyaudio module.")
            return 0

        result = test_audio.test_keyword_spotter("test/compiled_featurizer/mfcc",
                                                 "test/compiled_classifier/model",
                                                 self.categories_file,
                                                 self.wav_file,
                                                 threshold=0.95,
                                                 sample_rate=self.sample_rate)
        if not result or result[2] != "seven":
            raise Exception("Did not get expected prediction 'seven': {}".format(result))

        return 0


if __name__ == "__main__":
    unittest.main()
