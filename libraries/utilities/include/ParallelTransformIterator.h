////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParallelTransformIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <future>
#include <thread>
#include <vector>

#define DEFAULT_MAX_TASKS 8

namespace ell
{
namespace utilities
{
    /// <summary> A read-only forward iterator that transforms the items from an input collection. processes items in parallel when possible </summary>
    template <typename InputIteratorType, typename OutType, typename FuncType, int MaxTasks = 0>
    class ParallelTransformIterator
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="inIter"> An iterator for the input collection </param>
        /// <param name="transformFunction"> The function to apply to transform the input items</param>
        ParallelTransformIterator(InputIteratorType& inIter, FuncType transformFunction);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _currentIndex != _endIndex; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The result of applying the transformFunction on the current item in the input iterator </returns>
        OutType Get() const;

    private:
        InputIteratorType& _inIter;
        FuncType _transformFunction;

        mutable std::vector<std::future<OutType>> _futures; // mutable because future::get() isn't const
        mutable OutType _currentOutput;
        mutable bool _currentOutputValid;
        int _currentIndex = 0;
        int _endIndex = 0;
    };

    /// <summary> Convenience function for creating ParallelTransformIterators </summary>
    ///
    /// <param name="inIter"> An iterator for the input collection </param>
    /// <param name="transformFunction"> The function to apply to transform the input items</param>
    ///
    /// <returns> A ParallelTransformIterator over the input sequence using the specified transform function</returns>
    template <typename InputIteratorType, typename FuncType>
    auto MakeParallelTransformIterator(InputIteratorType& inIterator, FuncType transformFunction) -> ParallelTransformIterator<InputIteratorType, decltype(transformFunction(inIterator.Get())), FuncType>;
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // ParallelTransformIterator definitions
    //

    template <typename InputIteratorType, typename OutType, typename FuncType, int MaxTasks>
    ParallelTransformIterator<InputIteratorType, OutType, FuncType, MaxTasks>::ParallelTransformIterator(InputIteratorType& inIter, FuncType transformFunction) :
        _inIter(inIter),
        _transformFunction(transformFunction),
        _currentOutputValid(false),
        _currentIndex(0),
        _endIndex(-1)
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
} // namespace utilities
} // namespace ell

#pragma endregion implementation
