////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParallelTransformIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <future>
#include <vector>

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
}
}

#include "../tcc/ParallelTransformIterator.tcc"
