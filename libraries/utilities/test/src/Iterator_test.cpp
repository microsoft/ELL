
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Itetator_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Iterator_test.h"

// utilities
#include "IIterator.h"
#include "ParallelTransformIterator.h"
#include "StlContainerIterator.h"
#include "StlStridedIterator.h"
#include "TransformIterator.h"

// testing
#include "testing.h"

// stl
#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>

namespace ell
{
void TestIteratorAdapter()
{
    // utilities::StlContainerReferenceIterator test
    std::vector<int> vec{ 1, 2, 3, 4, 5, 6 };
    auto it = utilities::MakeStlContainerReferenceIterator(vec.begin(), vec.end());

    bool passed = true;
    size_t index = 0;
    while (it.IsValid())
    {
        passed = passed && (it.Get() == vec[index]);
        it.Next();
        index++;
    }

    testing::ProcessTest("utilities::StlContainerReferenceIterator.Get", passed);
    testing::ProcessTest("utilities::StlContainerReferenceIteratorlength", index == vec.size());
}

float twoPointFiveTimes(int x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return float(2.5 * x);
}

template <typename FuncType>
std::chrono::milliseconds::rep timeIt(FuncType fn)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    fn();
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

class MillisecondTimer
{
public:
    MillisecondTimer()
        : _start(std::chrono::system_clock::now()), _running(true) {}
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

void TestTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeStlContainerReferenceIterator(vec.begin(), vec.end());
    auto transIt = MakeTransformIterator(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    bool passed = true;
    int index = 0;
    while (transIt.IsValid())
    {
        passed = passed && transIt.Get() == float(2.5 * vec[index]);
        transIt.Next();
        index++;
    }

    testing::ProcessTest("utilities::TransformIterator.Get", passed);
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

void TestParallelTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeStlContainerReferenceIterator(vec.begin(), vec.end());
    auto transIt = MakeParallelTransformIterator(srcIt, twoPointFiveTimes);

    bool passed = true;
    MillisecondTimer timer;
    int index = 0;
    while (transIt.IsValid())
    {
        passed = passed && transIt.Get() == float(2.5 * vec[index]);
        transIt.Next();
        index++;
    }
    testing::ProcessTest("utilities::ParallelTransformIterator.Get", passed);
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

void TestStlStridedIterator()
{
    std::vector<double> vec(20);
    std::iota(std::begin(vec), std::end(vec), 1);

    auto begin = utilities::MakeStlStridedIterator(std::begin(vec), 2);
    auto end = utilities::MakeStlStridedIterator(std::end(vec), 2);
    int index = 0;
    for (auto it = begin; it != end; ++it, ++index)
    {
        testing::ProcessTest("utilities::StlStridedIterator element access", *it == vec[2 * index]);
    }
}
}
