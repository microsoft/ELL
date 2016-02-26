////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIteratorAdapter.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIterator.h"

// stl
#include <vector>

namespace utilities
{
    template <typename IteratorType, typename ValueType> // = decltype(*std::declval<IteratorType>())>
    class StlIteratorAdapter : public IIterator<ValueType>
    {
    public:
        StlIteratorAdapter(IteratorType begin, IteratorType end);

        virtual bool IsValid() const override;
        virtual void Next() override;
        virtual ValueType Get() const override;

    private:
        IteratorType _current;
        IteratorType _end;
    };

    template <typename IteratorType>
    auto MakeIteratorAdapter(IteratorType begin, IteratorType end)->StlIteratorAdapter<IteratorType, decltype(*begin)>;

    template <typename ContainerType>
    auto MakeIteratorAdapter(ContainerType& container)->StlIteratorAdapter<typename ContainerType::iterator, typename ContainerType::value_type>;
}

#include "../tcc/StlIteratorAdapter.tcc"