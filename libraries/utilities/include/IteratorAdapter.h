////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IteratorAdapter.h (utilities)
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
    template <typename T, typename IteratorType>
    class IteratorAdapter : public IIterator<T>
    {
    public:
        IteratorAdapter(IteratorType begin, IteratorType end) : _current(begin), _end(end)
        {
        }

        virtual bool IsValid() const override { return _current != _end; }
        virtual void Next() override { if (IsValid()) _current++; }
        virtual T Get() override { return *_current; }

    private:
        IteratorType _current;
        IteratorType _end;
    };

    template <typename IteratorType>
    auto MakeIterator(IteratorType begin, IteratorType end) -> IteratorAdapter<decltype(*begin), IteratorType>
    {
        return IteratorAdapter<decltype(*begin), IteratorType>(begin, end);
    }

    template <typename ContainerType>
    auto MakeIterator(ContainerType& container) -> IteratorAdapter<typename ContainerType::value_type, typename ContainerType::iterator>
    {
        return IteratorAdapter<typename ContainerType::value_type, typename ContainerType::iterator>(container.begin(), container.end());
    }
}
