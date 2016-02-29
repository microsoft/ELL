////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     ParallelTransformIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>
#include <future>

namespace utilities
{
    //
    // ParallelTransformIterator
    //
    template <typename InputIteratorType, typename OutType, typename Func, int MaxTasks=0>
    class ParallelTransformIterator
    {
    public:
        ParallelTransformIterator(InputIteratorType& inIter, Func transformFn);
        bool IsValid() const;
        void Next();
        OutType Get() const;

    private:
        InputIteratorType& _inIter;
        Func _transformFn;

        mutable std::vector<std::future<OutType>> _futures; // mutable because future::get() isn't const
        mutable OutType _currentOutput;
        mutable bool _currentOutputValid;
        int _currentIndex = 0;
        int _endIndex = 0;
    };

    // Convenience function for creating ParallelTransformIterators
    template <typename InputIteratorType, typename FnType>
    auto MakeParallelTransform(InputIteratorType& inIterator, FnType transformFn)->ParallelTransformIterator<InputIteratorType, decltype(transformFn(inIterator.Get())), FnType>;
}

#include "ParallelTransformIterator.tcc"
