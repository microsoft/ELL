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

#include <chrono>
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

#include <numeric>
using std::iota;

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

float twoPointFiveTimes(int x)
{
    std::this_thread::sleep_for(milliseconds(10));
    return float(2.5*x);
}

template <typename Func>
milliseconds::rep timeIt(Func fn)
{
    system_clock::time_point start = system_clock::now();
    fn();
    system_clock::time_point end = system_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

class MillisecondTimer
{
public:
    MillisecondTimer() : _start(system_clock::now()), _running(true) {}
    void Start()
    {
        _start = system_clock::now();
        _running = true;
    }

    void Stop()
    {
        _end = system_clock::now();
        _running = false;
    }

    milliseconds::rep Elapsed()
    {
        if (_running)
        {
            _end = system_clock::now();
        }

        return duration_cast<milliseconds>(_end - _start).count();
    }


private:
    system_clock::time_point _start;
    system_clock::time_point _end;
    bool _running;
};

void testTransformIterator()
{
    vector<int> vec(64);
    iota(vec.begin(), vec.end(), 5);

    auto srcIt = MakeIterator(vec.begin(), vec.end());
    auto transIt = MakeTransform(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    int index = 0;
    while(transIt.IsValid())
    {
        processTest("TransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
    auto elapsed = timer.Elapsed();
    cout << "Elapsed time: " << elapsed << " ms" << endl;
}

void testParallelTransformIterator()
{
    vector<int> vec(64);
    iota(vec.begin(), vec.end(), 5);

    auto srcIt = MakeIterator(vec.begin(), vec.end());
    auto transIt = MakeParallelTransform(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    int index = 0;
    while(transIt.IsValid())
    {
        processTest("ParallelTransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
    auto elapsed = timer.Elapsed();
    cout << "Elapsed time: " << elapsed << " ms" << endl;
}

/// Runs all tests
///
int main()
{
    // TODO test parser

    testIteratorAdapter();
    testTransformIterator();
    testParallelTransformIterator();

    if (testFailed())
    {
        return 1;
    }
    return 0;
}
