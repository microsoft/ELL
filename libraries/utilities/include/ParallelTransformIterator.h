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

#include "IIterator.h"

// stl
#include <vector>
#include <future>

namespace utilities
{
    //
    // ParallelTransformIterator
    //
    template <typename InType, typename OutType, typename Func, int MaxTasks=0>
    class ParallelTransformIterator : public IIterator<OutType>
    {
    public:
        ParallelTransformIterator(IIterator<InType>& inIter, Func transformFn);
        virtual bool IsValid() const override;
        virtual void Next() override;
        virtual OutType Get() override;

    private:
        IIterator<InType>& _inIter;
        Func _transformFn;

        std::vector<std::future<OutType>> _futures;
        OutType _currentOutput;
        bool _currentOutputValid;
        int _currentIndex = 0;
        int _endIndex = 0;
    };

    // Convenience function for creating ParallelTransformIterators
    template <typename InType, typename FnType>
    auto MakeParallelTransform(IIterator<InType>& inIterator, FnType transformFn) -> ParallelTransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>
    {
        return ParallelTransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>(inIterator, transformFn);
    }
}

#include "ParallelTransformIterator.tcc"
