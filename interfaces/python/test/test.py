from __future__ import print_function
import sys

sys.path.append('.')
sys.path.append('./..')
sys.path.append('./../Release')
sys.path.append('./../Debug')

tests = []

SkipTests = False
try:
    import ELL
    import functions_test
    import model_test
    import modelbuilder_test
    import common_test
    import trainers_test
    import predictors_test
    import nodes_test
    import linear_test
    import evaluators_test

    tests = [
        (functions_test.test,     "functions_test"),
        (model_test.test,         "model_test"),
        (common_test.test,        "common_test"),
        (trainers_test.test,      "trainers_test"),
        (predictors_test.test,    "predictors_test"),
        (nodes_test.test,         "nodes_test"),
        (linear_test.test,        "linear_test"),
        (evaluators_test.test,    "evaluators_test"),
        (modelbuilder_test.test,  "modelbuilder_test")
    ]
except ImportError:
     print("Import failed")
     SkipTests = True

def interface_test():
    rc = 0
    for (test, name) in tests:
        try:
            ans = test()
            if ans == 0:
                print(name, "passed")
            else:
                print(name, "failed")
            rc |= ans
        except Exception as e:
            print(name, "failed")
            print("exception:", e)
            rc = 1
    sys.exit(rc)

if not SkipTests:
    interface_test()

