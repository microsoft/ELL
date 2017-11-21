###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     test_compiled_model.py
#  Authors:  Chris Lovett
#  Requires: Python 3.x
#
###############################################################################
import os
import sys
import numpy as np
import time
import find_ell
import ELL

# import the compiled model
if not os.path.isdir("host"):
    print("'host' dir not found.")
    print("Please first compile the mnist.ell for your target host platform")
    sys.exit(1)

if not os.path.isdir("host/build"):
    print("'host/build' dir not found.")
    print("Please build the host project")
    sys.exit(1)

sys.path.append("host")
sys.path.append("host/build")
sys.path.append("host/build/Release")
try:
    import mnist
except:
    print("import mnist failed.")
    print("Please compile the mnist.ell for your target host platform")    
    sys.exit(1)


class CompiledTest:
    def __init__(self, model):
        """ Initialize the CompiledTest class by loading the compiled ELL model
        and getting the input size and output size """
        self.model = model
        input_shape = model.get_default_input_shape()
        print("input shape=%d,%d,%d" % (input_shape.rows, input_shape.columns,
                                        input_shape.channels))
        self.input_size = (input_shape.rows * input_shape.columns *
                          input_shape.channels)
        print("input size=%d" % (self.input_size))
        output_shape = model.get_default_output_shape()
        print("output shape=%d,%d,%d" % (output_shape.rows, 
                          output_shape.columns, output_shape.channels))
        self.output_size = int(output_shape.rows * output_shape.columns *
                           output_shape.channels)
        print("output_size=%d" % (self.output_size))
        self.output_buffer = model.DoubleVector(np.zeros((self.output_size)))
        self.copyto = False

    def load_data(self, filename):
        """ Load the given test dataset file as an AutoSupervisedDataset.
        This means the data file contains both the test data and expected 
        labels """
        print("Loading dataset: " + filename)
        self.dataset = ELL.AutoSupervisedDataset()
        self.dataset.Load(filename)

    def run_test(self):
        """ Run the test examples in the loaded dataset and compare the
        results with the expected label, and return the measured accuracy """
        total = self.dataset.NumExamples()
        true_positive = 0
        input_vector = ELL.DoubleVector()
        input_vector.resize(self.input_size)
        for i in range(total):
            example = self.dataset.GetExample(i)
            data = example.GetData()
            data.CopyTo(input_vector)
            self.model.predict(input_vector, self.output_buffer)
            a = np.asarray(self.output_buffer)
            answer = np.argmax(a)
            expected = example.GetLabel()
            if answer == expected:
                true_positive += 1
        accuracy = true_positive / total
        return accuracy

    def compute_accuracy(self):       
        """ Time the test and print the accuracy """     
        print("Computing accuracy...")
        start = time.time()
        accuracy = self.run_test()
        end = time.time()
        print("Accuracy %f" % (accuracy))
        total_time = end - start
        print("Total Predict time was %f seconds" % (total_time))
        avg_time = total_time / self.dataset.NumExamples()
        print("Average Predict time was %f per example" % (avg_time))


def main():
    test_file = "Test-28x28_cntk_text.ds"
    if not os.path.isfile(test_file):
        print("Could not find '%s'" % (test_file))
        print("See convert_dataset.py")
        sys.exit(1)

    test = CompiledTest(mnist)
    test.load_data(test_file)
    test.compute_accuracy()

if __name__ == "__main__":
    main()
