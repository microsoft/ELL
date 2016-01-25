// TransformIterator.h
#pragma once

#include "IIterator.h"

namespace utilities
{
    template <typename InType, typename OutType, typename Func>
    class TransformIterator : public IIterator<OutType>
    {
    public:
        TransformIterator(IIterator<InType>& inIter, Func transformFn);
        virtual bool IsValid() const override;
        virtual void Next() override;
        virtual OutType Get() override;

    private:
        IIterator<InType>& _inIter;
        Func _transformFn;
    };


    // Convenience function for creating TransformIterators
    template <typename InType, typename FnType>
    auto MakeTransform(IIterator<InType>& inIterator, FnType transformFn) -> TransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>
    {
        return TransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>(inIterator, transformFn);
    }
}

#include "TransformIterator.tcc"
