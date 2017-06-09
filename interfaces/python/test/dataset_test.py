from __future__ import print_function
import ELL

def GetItems(thing):
    '''
    Create a Python style generator for iteration through an instance of any class
    that exposes the following methods:
        GetIterator, IsValid, Get, Next
    The generator allows you to use Python methods for iterable entities
    such as "for x in blah: ...", "reduce", etc
    '''
    iterator = thing.GetIterator()
    while iterator.IsValid():
        yield iterator.Get()
        iterator.Next()

def GetExampleItems(example):
    '''
    Returns a Python generator for a list of IDataVector types
    '''
    for x in example.GetDataVector().ToDoubleArray():
        yield x

def exampleTest(example):
    # This is a fancy Python/Functional way getting s0 = sample count, s1 = sum of samples,
    # s2 = sum of square of samples.
    # This test the iterator
    (s0, s1, s2) = reduce(lambda (s0,s1,s2), x:(s0+1,s1+x,s2+x*x), GetExampleItems(example), (0,0,0))
    norm = 1.0 / float(s0)
    m1 = s1 * norm      # mean of the samples
    m2 = s2 * norm      # mean of the square of the samples
    var = m2 - m1*m1    # variance of the samples
    
    # Now test the IDataVector of the example
    xs = example.GetDataVector()
    # This is a function that will be used to count the number of samples that
    # are equal to zero
    def g(count,x):
        '''If x is zero then increment the count and return it otherwise return the original count'''
        if x != 0:
            return count + 1
        else:
            return count
    # Use reduce to count the number of zeros in the samples
    numNonZeros = reduce(g, GetExampleItems(example), 0)
    # Check this against what the example object says
    diff = numNonZeros - xs.NumNonzeros()
    # Check Norm2 agains our sum of squares
    norm2 = xs.Norm2()
    print("%7.3f %7.3f %7.3f %d" % (m1, var, norm2 - s2, diff))

def work():
    dataset = ELL.GetDataset("../../../../examples/data/testData.txt")
    # Count the number of examples by iteration and print
    print("Number of Examples:", reduce(lambda a,b:a+1, GetItems(dataset), 0))
    # Compare this with what the dataset object reports
    print("NumExamples:", dataset.NumExamples())
    for example in GetItems(dataset): 
        exampleTest(example)

def test():
    try:
        work()
        return 0
    except:
        return 1

