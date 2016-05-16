////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IntegerList.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IntegerList.h"

// stl
#include <stdexcept>

namespace utilities
{
    IntegerList::Iterator::Iterator(const vector_iterator& begin, const vector_iterator& end) : _begin(begin), _end(end)
    {}

    void IntegerList::Reserve(uint64_t size)
    {
        _list.reserve(size);
    }

    uint64_t IntegerList::Max() const
    {
        if(_list.size()==0)
        {
            throw std::runtime_error("Can't get max of empty list");
        }

        return _list[Size()-1];
    }

    void IntegerList::Append(uint64_t value)
    {
        _list.push_back(value);
    }

    IntegerList::Iterator IntegerList::GetIterator() const
    {
        return Iterator(_list.cbegin(), _list.cend());
    }
}
