###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     test_reference_model.py
#  Authors:  Chris Lovett
#  Requires: Python 3.x
#
###############################################################################
import os
import sys
import time
import find_ell
import ELL


class ReferenceTest:
    def __init__(self, model_file):
        """ Initialize the ReferenceTest class by loading the ELL model 
        and getting the input size and output size """
        print("Loading model: " + model_file)
        self.mnist = ELL.ELL_Map(model_file)
        input_shape = self.mnist.GetInputShape()
        self.input_size = (input_shape.rows * input_shape.columns * 
                          input_shape.channels)
        print("input size=%d" % (self.input_size))
        output_shape = self.mnist.GetOutputShape()
        self.output_size = (output_shape.rows * output_shape.columns * 
                           output_shape.channels)


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
        for i in range(total):
            example = self.dataset.GetExample(i)
            result = self.mnist.ComputeDouble(example.GetData())
            answer = ELL.DoubleArgmax(result)
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

    model_file = "mnist.ell"
    if not os.path.isfile(model_file):
        print("Could not find '%s'" % (model_file))
        print("See train_protonn.py")
        sys.exit(1)

    test = ReferenceTest(model_file)
    test.load_data(test_file)
    test.compute_accuracy()


if __name__ == "__main__":
    main()
