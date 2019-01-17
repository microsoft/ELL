#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     pitest.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################
import argparse
import sys
import unittest
import logging

import drivetest
import logger

# Try to import CNTK and ELL. If either don't exist it means they have not
# been built, so don't run the tests.
SkipTests = False
SkipFullModelTests = False
cluster = None
password = None
key = None
targets = ["pi3", "pi0"]
log = logger.get()
gitrepo = None


class PiTestBase(unittest.TestCase):
    def setUp(self):
        global SkipTests
        if SkipTests:
            self.skipTest('Module not tested, ELL module missing')

    def test_raspberryPi(self):
        global cluster
        for target in targets:
            log.info("=============== Testing platform: {} ===================".format(target))
            with drivetest.DriveTest(cluster=cluster, target=target, target_dir="/home/pi/" + target,
                                     username="pi", password=password, expected="coffee mug", timeout=300, apikey=key,
                                     gitrepo=gitrepo) as driver:
                driver.run_test()


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser(
        description="ELL python unit tests for Raspberry Pi",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument(
        '--cluster', help='HTTP address of cluster manager')
    parser.add_argument(
        '--password', help='The raspberry pi password', default="raspberry")
    parser.add_argument(
        '--key', help='The raspberry pi cluster manager api key')
    parser.add_argument(
        '--gitrepo', help='The URL from where to get test models')
    parser.add_argument(
        '--targets', help='The raspberry pi targets (pi3, pi0, etc)', default="pi0,pi3")

    args, argv = parser.parse_known_args()
    cluster = args.cluster
    password = args.password
    gitrepo = args.gitrepo
    if args.targets:
        targets = [x.strip() for x in args.targets.split(',')]
    key = args.key

    unittest.main(argv=[sys.argv[0]] + argv)
