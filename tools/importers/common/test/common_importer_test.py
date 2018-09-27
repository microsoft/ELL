###############################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     common_importer_test.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, cntk-2.4
#
###############################################################################

import json
import unittest

from download_helper import *
import find_ell

class EllImporterTestBase(unittest.TestCase):
    def setUp(self):
        pass

    def get_test_model_repo(self):
        config_file = os.path.join(find_ell.find_ell_build(), "config.json")
        if os.path.isfile(config_file):
            with open(config_file, "r") as f:
                config = json.load(f)
                if "test_models_repo" in config:
                    return clone_repo(config["test_models_repo"], get_home_path())
            return None
        else:
            raise Exception("Missing config.json file: {}".format(config_file))