////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParallelTransformIterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <thread>

#define DEFAULT_MAX_TASKS 8

namespace ell
{
namespace utilities
{
    //
    // ParallelTransformIterator definitions
    //

    template <typename InputIteratorType, typename OutType, typename FuncType, int MaxTasks>
    ParallelTransformIterator<InputIteratorType, OutType, FuncType, MaxTasks>::ParallelTransformIterator(InputIteratorType& inIter, FuncType transformFunction)
        : _inIter(inIter), _transformFunction(transformFunction), _currentOutputValid(false), _currentIndex(0), _endIndex(-1)
    {
        // Fill the buffer with futures that are the result of calling std::async(transformFunction) on inIter
        int maxTasks = MaxTasks == 0 ? std::thread::hardware_concurrency() : MaxTasks;
        if (maxTasks == 0) // if std::thread::hardware_concurrency isn't implemented, use DEFAULT_MAX_TASKS tasks (maybe this should be 1)
        {
            maxTasks = DEFAULT_MAX_TASKS;
        }

        _futures.reserve(maxTasks);
        for (int index = 0; index < maxTasks; index++)
        {
            if (!_inIter.IsValid())
            {
                break;
            }

            _futures.emplace_back(std::async(std::launch::async, _transformFunction, _inIter.Get()));
            _inIter.Next();
        }
    }

    template <typename InputIteratorType, typename OutType, typename FuncType, int MaxTasks>
    void ParallelTransformIterator<InputIteratorType, OutType, FuncType, MaxTasks>::Next()
    {
        if (!IsValid())
        {
            return;
        }
        _currentOutputValid = false;

        // If necessary, create new std::future to handle next input
        if (_inIter.IsValid())
        {
            _futures[_currentIndex] = std::async(std::launch::async, _transformFunction, _inIter.Get());
            _inIter.Next();
        }
        else
        {
            if (_endIndex < 0) // Check if we've already noted the end index
            {
                _endIndex = _currentIndex;
            }
        }
        _currentIndex = (_currentIndex + 1) % _futures.size();
    };

    template <typename InputIteratorType, typename OutType, typename FuncType, int MaxTasks>
    OutType ParallelTransformIterator<InputIteratorType, OutType, FuncType, MaxTasks>::Get() const
    {
        // Need to cache output of current std::future, because calling std::future::get() twice is an error
        if (!_currentOutputValid)
        {
            _currentOutput = _futures[_currentIndex].get();
            _currentOutputValid = true;
        }

        return _currentOutput;
    }

    template <typename InputIteratorType, typename FuncType>
    auto MakeParallelTransformIterator(InputIteratorType& inIterator, FuncType transformFunction) -> ParallelTransformIterator<InputIteratorType, decltype(transformFunction(inIterator.Get())), FuncType>
    {
        using OutType = decltype(transformFunction(inIterator.Get()));
        return ParallelTransformIterator<InputIteratorType, OutType, FuncType>(inIterator, transformFunction);
    }
}
}
