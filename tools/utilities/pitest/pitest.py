#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     pitest.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
import argparse
import drivetest
import sys
import unittest
import logging

# Try to import CNTK and ELL. If either don't exist it means they have not
# being built, so don't run the tests.
SkipTests = False
SkipFullModelTests = False
cluster = None

class PiTestBase(unittest.TestCase):
    def setUp(self):
        global SkipTests
        if SkipTests:
            self.skipTest('Module not tested, ELL module missing')

    def test_raspberryPi(self):
        global cluster
        with drivetest.DriveTest(cluster=cluster, target="pi3",
            target_dir="/home/pi/pi3", username="pi", password="raspberry",
            expected="coffee mug", timeout=300) as driver:
            driver.run_test()


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    
    parser = argparse.ArgumentParser(
        description="ELL python unit tests for Raspberry Pi",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument(
        '--cluster', help='HTTP address of cluster manager')

    args, argv = parser.parse_known_args()
    cluster = args.cluster
    
    unittest.main(argv=[sys.argv[0]] + argv)
