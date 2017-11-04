####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     pitest.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
import unittest
import sys
import drivetest
import argparse

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
        with drivetest.DriveTest() as driver:
            driver.parse_command_line(["--cluster", cluster])
            driver.run_test()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="ELL python unit tests for Raspberry Pi",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument(
        '--cluster', help='HTTP address of cluster manager')

    args, argv = parser.parse_known_args()
    cluster = args.cluster
    
    unittest.main(argv=[sys.argv[0]] + argv)
