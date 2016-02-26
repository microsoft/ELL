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

#include "IIterator.h"

// stl
#include <utility>

namespace utilities
{
    template <typename InType, typename OutType, typename Func>
    class TransformIterator : public IIterator<OutType>
    {
    public:
        TransformIterator(IIterator<InType>& inIter, Func transformFn);
        virtual bool IsValid() const override;
        virtual void Next() override;
        virtual OutType Get() const override;

    private:
        IIterator<InType>& _inIter;
        Func _transformFn;
    };

    // Convenience function for creating TransformIterators
    template <typename InType, typename FnType>
    auto MakeTransform(IIterator<InType>& inIterator, FnType transformFn)->TransformIterator<InType, decltype(transformFn(std::declval<InType>())), FnType>;
}

#include "TransformIterator.tcc"
