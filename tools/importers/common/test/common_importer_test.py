#!/usr/bin/env python3
###############################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     common_importer_test.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, cntk-2.4
#
###############################################################################
import os
import json
import unittest

from download_helper import clone_repo, get_home_path
import find_ell
import numpy as np


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

    def IsEqualWithinTolerance(self, a, b, tol):
        return (a - b) < tol and (b - a) < tol

    def IsEqual(self, a, b, tol=1.0e-8):
        # Instances of the same swig proxy types (e.g. DoubleVector) resolve to different type()'s
        if 'Vector' in str(type(a)):
            if str(type(a)) != str(type(b)):
                return False
            if a.size() != b.size():
                return False
            if a.size() == 0:
                return True
            for i in range(a.size()):
                if not self.IsEqual(a[i], b[i]):
                    return False
            return True
        elif type(a) != type(b):
            return False
        if type(a) is list:
            if len(a) != len(b):
                return False
            if len(a) == 0:
                return True
            for i in range(len(a)):
                if not self.IsEqual(a[i], b[i]):
                    return False
            return True
        else:
            if type(a) is float:
                return self.IsEqualWithinTolerance(a, b, tol)
            elif isinstance(a, np.ndarray):
                np.testing.assert_almost_equal(a, b)
                return True
            else:
                return a == b

    def AssertEqual(self, a, b, message, tolerance=1.0e-8):
        if not self.IsEqual(a, b, tolerance):
            raise Exception(message)