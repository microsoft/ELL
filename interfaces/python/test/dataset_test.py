import EMLL

def GetItems(thing):
    iterator = thing.GetIterator()
    while iterator.IsValid():
        yield iterator.Get()
        iterator.Next()

def GetExampleItems(example):
    for x in example.GetDataVector().ToArray():
        yield x

def f(example):
    (s0, s1, s2) = reduce(lambda (s0,s1,s2), x:(s0+1,s1+x,s2+x*x), GetExampleItems(example), (0,0,0))
    norm = 1.0 / float(s0)
    m1 = s1 * norm
    m2 = s2 * norm
    var = m2 - m1*m1
    xs = example.GetDataVector()
    def g(count,x):
        if x != 0:
            return count + 1
        else:
            return count
    numNonZeros = reduce(g, GetExampleItems(example), 0)
    diff = numNonZeros - xs.NumNonzeros()
    norm2 = xs.Norm2()
    print "%7.3f %7.3f %7.3f %d" % (m1, var, norm2 - s2, diff)

def work():
    dataset = EMLL.GetDataset("../../../../examples/data/testData.txt")
    print "Number of Examples:", reduce(lambda a,b:a+1, GetItems(dataset), 0)
    for example in GetItems(dataset): 
        f(example)

def test():
    try:
        work()
        return 0
    except:
        return 1

