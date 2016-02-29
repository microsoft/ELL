////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TransformIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <utility>

namespace utilities
{
    template <typename InputIteratorType, typename OutType, typename Func>
    class TransformIterator
    {
    public:
        TransformIterator(InputIteratorType& inIter, Func transformFn);
        bool IsValid() const;
        void Next();
        OutType Get() const;

    private:
        InputIteratorType& _inIter;
        Func _transformFn;
    };

    // Convenience function for creating TransformIterators
    template <typename InputIteratorType, typename FnType>
    auto MakeTransform(InputIteratorType& inIterator, FnType transformFn)->TransformIterator<InputIteratorType, decltype(transformFn(inIterator.Get())), FnType>;
}

#include "TransformIterator.tcc"
