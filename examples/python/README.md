Python example projects
=======================

SGD
---

swig_test
---------
swig_test is a suite of Python test scripts which are run as part of ctest.
It is easy to another test to the suite.

## Adding another test file to the swig_test suite

1. Create a Python test script file

   For the purposes of this discussion I shall assume that I will be adding a Python test script called "sometest.py". This file must export a function called "test" that takes () as input and returns either 0 for success or 0 for failure. The test function should not propagate exceptions.

2. Making sure your python file is copied to the testing environment

   Add somptest.py to PYTHON_EXAMPLES in EMLL/Examples/python/CMakeLists.txt

3. Add sometest.test() to the swig_test suite

   Modify swigtest.py by adding "import sometest" to the header and adding "sometest.test" to the array called "tests".
