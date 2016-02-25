////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     test.cpp (utilities_test)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIterator.h"
#include "TransformIterator.h"
#include "ParallelTransformIterator.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>
#include <thread>

void testIteratorAdapter()
{
    // utilities::IteratorAdapter test
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };
    auto it = utilities::MakeIterator(vec.begin(), vec.end());
    
    int index = 0;
    while(it.IsValid())
    {
        testing::ProcessTest("utilities::IteratorAdapter.Get", it.Get() == vec[index]);
        it.Next();
        index++;
    }
            
    testing::ProcessTest("utilities::IteratorAdapter length", index == vec.size());
}

float twoPointFiveTimes(int x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return float(2.5*x);
}

template <typename Func>
std::chrono::milliseconds::rep timeIt(Func fn)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    fn();
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

class MillisecondTimer
{
public:
    MillisecondTimer() : _start(std::chrono::system_clock::now()), _running(true) {}
    void Start()
    {
        _start = std::chrono::system_clock::now();
        _running = true;
    }

    void Stop()
    {
        _end = std::chrono::system_clock::now();
        _running = false;
    }

    std::chrono::milliseconds::rep Elapsed()
    {
        if (_running)
        {
            _end = std::chrono::system_clock::now();
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
    }


private:
    std::chrono::system_clock::time_point _start;
    std::chrono::system_clock::time_point _end;
    bool _running;
};

void testTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeIterator(vec.begin(), vec.end());
    auto transIt = MakeTransform(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    int index = 0;
    while(transIt.IsValid())
    {
        testing::ProcessTest("utilities::TransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

void testParallelTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeIterator(vec.begin(), vec.end());
    auto transIt = MakeParallelTransform(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    int index = 0;
    while(transIt.IsValid())
    {
        testing::ProcessTest("utilities::ParallelTransformIterator.Get", transIt.Get() == float(2.5*vec[index]));
        transIt.Next();
        index++;
    }
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

/// Runs all tests
///
int main()
{
    testIteratorAdapter();
    testTransformIterator();
    testParallelTransformIterator();

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}
