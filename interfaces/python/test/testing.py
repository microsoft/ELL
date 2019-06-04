import math
import numpy as a


class Testing(object):
    @staticmethod
    def IsEqualWithinTolerance(a, b, tol):
        t = a - b
        scale = 1.0
        diff = abs(t)
        if abs(a) > 10 and abs(b) > 10:
            scale = math.pow(10, int(math.log10(abs(a))))
            diff /= scale
        return diff < tol

    @staticmethod
    def ToList(a):
        if type(a) is float:
            return [a]
        if type(a) is int:
            return [a]
        if type(a) is bool:
            return [a]
        return list(a)

    @staticmethod
    def IsEqual(a, b, tol=1.0e-8):
        # Instances of the same swig proxy types (e.g. DoubleVector) resolve to different type()'s
        a = Testing.ToList(a)
        b = Testing.ToList(b)
        if len(a) != len(b):
            return False
        if len(a) == 0:
            return True
        for i in range(len(a)):
            if not Testing.IsEqualWithinTolerance(a[i], b[i], tol):
                return False
        return True

    def __init__(self):
        self.testFailedFlag = False

    def ProcessTest(self, testDescription, success):
        print("%s ... " % (testDescription), end="")
        if success:
            print("Passed")
        else:
            print("Failed")
            self.testFailedFlag = True

    def DidTestFail(self):
        return self.testFailedFlag
