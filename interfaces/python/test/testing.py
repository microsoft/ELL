class Testing(object):
    @staticmethod
    def IsEqualWithinTolerance(a, b, tol):
        t = a - b
        return (a-b) < tol and (b-a) < tol
    @staticmethod
    def IsEqual(a, b, tol=1.0e-8):
        if type(a) != type(b):
            return False
        if type(a) is list:
            if len(a) != len(b):
                return False
            if len(a) == 0:
                return True
            for i in range(len(a)):
                if a[i] != b[i]:
                    return False
            return True
        else:
            if type(a) is float:
                return Testing.IsEqualWithinTolerance(a,b,tol)
            else:
                return a == b
    def __init__(self):
        self.testFailedFlag = False
    def ProcessTest(self, testDescription, success):
        print "%s ..." % (testDescription),
        if success:
            print "Passed"
        else:
            print "Failed"
            self.testFailedFlag = True
    def DidTestFail(self):
        return self.testFailedFlag

