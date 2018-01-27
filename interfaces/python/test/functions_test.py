import ell
from math import exp, log
from testing import Testing

hingeLossInput =  [(+2, +1), (-2, -1), (+2, -1), (-2, +1), (+0, +1), (+0, -1)]
logLossInput = [(+2,+1), (-2,-1), (+2, -1), (-2, 1), (0, +1), (0, -1)]
squareLossInput = [(4, 4), (4, 2), (2, 4)]

def getData(f, dataInput): return [(y, t, f(y, t)) for (y, t) in dataInput]

def compareTest(testing, name, compare, data):
    def f(x, y, z):
        desc = "Testing %s(%d, %d)" %(name, x, y)
        testing.ProcessTest(desc, testing.IsEqual(compare(x,y), z))
    for (x, y, z) in data: f(x, y, z)

def hingeLossTest(testing):
    def f(y, t): return float(max(0, 1 - t*y))      # reference hingeloss
    def df(y, t):                                   # derivative
        if t*y <= 1.0: return -float(t)
        else: return 0.0
    x = ell.math.HingeLoss()
    compareTest(testing, "HingeLoss.Evaluate",  x.Evaluate, getData(f, hingeLossInput))
    compareTest(testing, "HingeLoss.GetDerivative", x.GetDerivative, getData(df, hingeLossInput))

def logLossTest(testing):
    def f(y, t): return log(1.0 + exp(-y*t))        # reference logloss
    def df(y,t): return (-t/(1.0 + exp(y*t)))       # derivative
    x = ell.math.LogLoss()
    compareTest(testing, "LogLoss.Evaluate", x.Evaluate,  getData(f, logLossInput))
    compareTest(testing, "LogLoss.GetDerivative", x.GetDerivative, getData(df, logLossInput))

def squaredLossTest(testing):
    def f(y, t): return 0.5 * (y - t) * (y - t)
    def df(y, t): return float(y - t)
    x = ell.math.SquaredLoss()
    compareTest(testing, "SquaredLoss.Evaluate", x.Evaluate, getData(f, squareLossInput))
    compareTest(testing, "SquaredLoss.GetDerivative", x.GetDerivative, getData(df, squareLossInput))

def test():
    testing = Testing()
    hingeLossTest(testing)
    logLossTest(testing)
    squaredLossTest(testing)
    if testing.DidTestFail():
        return 1
    else:
        return 0
