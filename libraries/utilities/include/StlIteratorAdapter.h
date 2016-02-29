////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIteratorAdapter.h (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>

namespace utilities
{
    template <typename IteratorType, typename ValueType> // = decltype(*std::declval<IteratorType>())>
    class StlIteratorAdapter
    {
    public:
        StlIteratorAdapter(IteratorType begin, IteratorType end);
        
        bool IsValid() const;
        void Next();
        ValueType Get() const;

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
