// ParallelTransformIterator
#pragma once

#include "IIterator.h"

#include <vector>
using std::vector;

#include <future>
using std::future;

namespace utilities
{
    //
    // ParallelTransformIterator
    //
    template <typename InType, typename OutType, typename Func>
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

        const int _maxSize = 4;
        vector<future<OutType>> _futures;
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
