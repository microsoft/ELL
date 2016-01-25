// test.cpp

// testing
#include "testing.h"
using testing::isEqual;
using testing::processTest;
using testing::testFailed;

#include "IIterator.h"
using utilities::IIterator;

#include "TransformIterator.h"
using utilities::TransformIterator;

#include "ParallelTransformIterator.h"
using utilities::ParallelTransformIterator;

// stl
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

template <typename T, typename IteratorType>
class IteratorAdapter : public IIterator<T>
{
public:
    IteratorAdapter(IteratorType begin, IteratorType end) : _current(begin), _end(end)
    {
    }

    virtual bool IsValid() const override { return _current != _end; }
    virtual void Next() override { if(IsValid()) _current++; }
    virtual T Get() override { return *_current; }

private:
    IteratorType _current;
    IteratorType _end;
};

template <typename IteratorType>
auto MakeIterator(IteratorType begin, IteratorType end) -> IteratorAdapter<decltype(*begin), IteratorType>
{
    return IteratorAdapter<decltype(*begin), IteratorType>(begin, end);
}


void testIteratorAdapter()
{
    // IteratorAdapter test
    vector<int> vec { 1, 2, 3, 4, 5, 6 };
    auto it = MakeIterator(vec.begin(), vec.end());
    
    int index = 0;
    while(it.IsValid())
    {
        processTest("IteratorAdapter.Get", it.Get() == vec[index]);
        it.Next();
        index++;
    }
            
    processTest("IteratorAdapter length", index == vec.size());
}

void testTransformIterator()
{
    vector<int> vec { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto twoPointFiveTimes = [](int x){ return float(2.5*x);};
    auto srcIt = MakeIterator(vec.begin(), vec.end());
    auto transIt = MakeTransform(srcIt, twoPointFiveTimes);


    int index = 0;
    while(transIt.IsValid())
    {
        processTest("TransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
}


void testParallelTransformIterator()
{
    vector<int> vec { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto srcIt = MakeIterator(vec.begin(), vec.end());
    auto twoPointFiveTimes = [](int x){ return float(2.5*x);};
    auto transIt = MakeParallelTransform(srcIt, twoPointFiveTimes);

    int index = 0;
    while(transIt.IsValid())
    {
        processTest("ParallelTransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
}


/// Runs all tests
///
int main()
{
    // TODO test parser

    testIteratorAdapter();
    testTransformIterator();
    testParallelTransformIterator();

    // TransformedIterator test

    if (testFailed())
    {
        return 1;
    }
    return 0;
}
