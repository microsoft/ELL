####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     modelTester.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse
import cv2
import numpy as np
import time
import json
import demoHelper 

script_path = os.path.dirname(os.path.abspath(__file__))

# Helper class that contains test results for the top N test.
class TestResults:
    def __init__(self, n, size):
        self.test_top_n = n
        self.tests_passed = 0
        self.tests_failed = 0
        self.confusion_matrix = np.zeros((size, size))
        self.results = []

    def record_result(self, passed, test_index, prediction, probability, expected):
        msg = "passed"
        if passed:
            self.tests_passed += 1
        else:
            msg = "failed"
            self.tests_failed += 1
        
        ok = 1
        if not passed:
            ok = 0
        self.results.append((test_index, ok, prediction, probability))
         
        total = self.tests_passed + self.tests_failed
        pass_rate = (self.tests_passed * 100) / total  
        print("  Top %d test %s (%d), current pass rate %d" % (self.test_top_n, msg, test_index, pass_rate))

    def resize(self, size):
        # used when we are grouping results into smaller set.
        self.confusion_matrix = np.zeros((size, size))

    def record_confusion(self, prediction, expected):
        self.confusion_matrix[prediction,expected] += 1

    def report_results(self, batch):
        print("======top %d results for batch %d ==============================================================================================" % (self.test_top_n, batch))
        total = self.tests_passed + self.tests_failed
        if (total > 0):
            pass_rate = (self.tests_passed * 100) / total
            result = "%d tests passed out of total %d, (%d tests failed) a pass rate of %d " % (self.tests_passed, total, self.tests_failed, pass_rate)
            print(result + "%")

        if not self.confusion_matrix is None:
            np.savetxt("top" + str(self.test_top_n) + "_batch_" + str(batch) + ".txt", self.confusion_matrix)

        with open("top" + str(self.test_top_n) + "_batch_" + str(batch) + ".json", 'w', encoding='utf-8', newline='\r\n') as outfile:
            json.dump(self.results, outfile, ensure_ascii=False, indent=2)

# Helper class that interfaces with opencv and provides handy conversion from opencv to ELL buffers and 
# rendering utilties
class ModelTester(demoHelper.DemoHelper):
    def __init__(self, threshold=0.0):

        super(ModelTester,self).__init__(threshold)
        self.val_map = {}
        self.val_pos = 0
        self.val_end = 0
        self.val_step = 1
        self.val_labels = []     
        self.map_entry = None
        self.test_complete = False
        self.batch = 0
        self.automatic = True
        self.groups = None
        self.group_names = {}
        self.top1 = None
        self.topN = None
        self.test_top_n = 1
        self.predict_time = 0
        self.predict_count = 0
        self.has_prediction = False
        self.reverse = False

    def add_arguments(self, arg_parser):
        super(ModelTester,self).add_arguments(arg_parser)    
        # args to setup test run
        arg_parser.add_argument("--truth", help="path to a tsv file, each line contains two values, the file name of the image and the integer classification value")
        arg_parser.add_argument("--truthlabels", help="path to a labels for the truth file (in case these are different from your model labels)")
        arg_parser.add_argument("--top", type=int, help="how many of the top labels to include in the test (default 1)", default=1)
        arg_parser.add_argument("--batch", type=int, help="which batch out of 10 batches to execute (default all)", default=0)
        arg_parser.add_argument("--groups", nargs='*', help="filenames that provide a grouping for the labels")
    

    def initialize(self, args):
        super(ModelTester,self).initialize(args)

        # remove camera default
        self.camera = None

        # test args
        self.batch = args.batch
        
        if args.truth is None:
            print("--truth argument is missing")
            return False

        if self.image_folder is None or not os.path.isdir(self.image_folder):
            print("image --folder % not found" % (self.image_folder))
            return False

        self.val_map_dir = os.path.dirname(args.truth)
        self.val_map = self.load_truth(args.truth)
        if args.truthlabels:
            self.val_labels = self.load_labels(args.truthlabels)
        else:
            self.val_labels = self.labels

        self.val_end = len(self.val_map)
        self.batch_size = self.val_end / 10
        if self.batch > 0:
            self.val_pos = (self.batch - 1) * self.batch_size
            self.val_end = self.val_pos + self.batch_size

        if self.reverse:
            temp = self.val_pos            
            self.val_step = -1  # go backwards        
            self.val_pos = self.val_end - 1
            self.val_end = temp - 1
            print("starting batch %d at %d and going to %d with step %d" % (self.batch, self.val_pos, self.val_end, self.val_step))

        self.top1 = TestResults(1, self.output_size)
        self.test_top_n = args.top
        if self.test_top_n != 1 and self.test_top_n != 0:
            self.topN = TestResults(args.top, self.output_size)

        self.load_group_labels(args.groups)

        self.automatic = True
        print(time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime()))
        return True

    def load_group_labels(self, filenames):
        if filenames is None:
            return None
        groups = []
        i = 0
        for name in filenames:
            groups.append(self.load_labels(name))
            self.group_names[i] = os.path.basename(name)
            i += 1
        
        self.group_names[i] = "other"
        
        size = len(groups)+1
        self.top1.resize(size)
        if self.topN != None:
            self.topN.resize(size)
        self.groups = groups

    def init_image_source(self):        
        self.frame = self.load_next_image()

    def load_next_image(self):
        self.recorded = False # waiting for user input.
        frame = None
        while frame == None and self.val_pos != self.val_end:
            self.map_entry = self.val_map[self.val_pos]
            self.val_pos += self.val_step
            path = os.path.join(self.val_map_dir, self.map_entry[0])
            if os.path.isfile(path):
                frame = cv2.imread(path)
                if (type(frame) == type(None)):
                    print('image from %s failed to load' % (path))
                return frame
            else:
                print("image file '%s' does not exist" % (path))
        
        self.test_complete = True
        return self.frame    

    def draw_label(self, image, label):
        """Helper to draw text onto an image"""
        cv2.rectangle(
            image, (0, 0), (image.shape[1], 40), (50, 200, 50), cv2.FILLED)
        cv2.putText(image, label, (10, 25),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 0), 2, cv2.LINE_AA)
        if self.map_entry:
            current_label = self.get_current_label()
            truth_label = self.get_truth_label(self.map_entry[1])
            color = (50, 50, 200)
            if self.labels_match(current_label, truth_label):
                color = (200, 50, 50)
            cv2.rectangle(
                image, (0, image.shape[0] - 40), (image.shape[1],  image.shape[0]), color, cv2.FILLED)
            cv2.putText(image, truth_label, (10, image.shape[0] - 15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 0), 2, cv2.LINE_AA)
        return

    def labels_match(self, a, b):
        x = [s.strip().lower() for s in a.split(',')]
        y = [s.strip().lower() for s in b.split(',')]
        for w in x:
            if (w in y):
                return True
        return False

    def get_current_label(self):
        topN = self.get_top_n(self.results, 1)
        if len(topN) > 0:
            return self.get_label(topN[0][0])
        return "unknown"

    def done(self):
        # on slow devices this helps let the images to show up on screen
        result = False

        if self.test_complete:
            return True
    
        if self.automatic and self.has_prediction:
            self.record_result()

    def label_in_group(self, label, group):
        for x in group:
            if self.labels_match(label, x):
                return True
        return False

    def get_truth_label(self, i):
        if i < len(self.val_labels):
            return self.val_labels[i]
        return ""

    def get_grouped_result(self, prediction):
        label = self.get_label(prediction)
        for i in range(len(self.groups)):
            g = self.groups[i]
            if self.label_in_group(label, g):
                return i
        return len(self.groups)

    def predict(self, data):
        now = time.time()
        super(ModelTester,self).predict(data)
        now2 = time.time()
        self.predict_time += now2 - now
        self.predict_count += 1
        self.has_prediction = True
        print("Test %d predicted in %f seconds" % (self.val_pos, now2 - now))

    def record_result(self):
        self.has_prediction = False
        # check current prediction matches the truth.
        expected = self.map_entry[1]
        expected_label =  self.get_truth_label(expected)
        if not self.groups is None:
            expected = self.get_grouped_result(expected)
            expected_label =  self.group_names[expected]

        topN = self.get_top_n(self.results, self.test_top_n)
        winner = None
        actual = []
        first = True
        topNPrediction = None
        topNProbability = 0
        for top in topN:
            prediction = top[0]
            label = self.get_label(prediction)
            if not self.groups is None:
                prediction = self.get_grouped_result(prediction)
                label = self.group_names[prediction]

            matches = self.labels_match(label, expected_label)
            if first:
                topNPrediction = prediction
                topNProbability = top[1]
                first = False
                self.top1.record_confusion(prediction, expected)
                self.top1.record_result(matches, self.val_pos, prediction, topNProbability, expected_label)
            
            actual.append(label)
            if matches and winner is None:
                winner = top
                topNPrediction = prediction
                topNProbability = top[1]
                break
    
        print("  " + ",".join(actual))    
        if self.topN != None:
            self.topN.record_confusion(topNPrediction, expected)
            self.topN.record_result(winner != None, self.val_pos, topNPrediction, topNProbability, expected_label)    
        
        if winner == None:
            print("  ====> Expected=" + expected_label)
            
        self.recorded = True
        self.frame = self.load_next_image()

    def load_truth(self, fileName):
        result = {}
        if fileName != None:
            if not os.path.isfile(fileName):
                print("Could not find truth map: ", fileName)
                sys.exit(1)
                
            print("loading truth:" + fileName)
            count = 0
            with open(fileName) as f:
                for line in f:  
                    if line != "":
                        name, index = line.split('\t')
                        result[count] = (name, int(index))
                        count = count + 1

        return result

    def report_times(self):
        # test complete
        self.top1.report_results(self.batch)
        if self.topN != None:
            self.topN.report_results(self.batch)
            
        total_seconds = self.predict_time
        print(time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime()))
        print("Total prediction time is %d seconds" % (total_seconds))
        print("Average time per image %f seconds" % (total_seconds / self.predict_count))



def main(args):
    helper = ModelTester()
    helper.parse_arguments(args, 
            "Runs the given ELL model passing images from a test folder where the given truth file contains the name of\n"
            "each image and the correct prediction (separated by a tab character).\n"
            "Example:\n"
            "   python batchtest.py darknetImageNetLabels.txt --truth val_map.txt --folder images --model darknet.ell\n"
            )
        

    # Initialize image source
    helper.init_image_source()

    lastPrediction = ""

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)

        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        helper.predict(data)

    helper.report_times()

if __name__ == "__main__":
    args = sys.argv
    args.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    main(args)
