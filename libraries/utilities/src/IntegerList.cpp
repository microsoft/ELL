////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IntegerList.cpp (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IntegerList.h"

// stl
#include <stdexcept>

namespace utilities
{
    bool IntegerList::Iterator::IsValid() const
    {
        return _begin < _end;
    }

    void IntegerList::Iterator::Next()
    {
        ++_begin;
    }

    uint64 IntegerList::Iterator::Get() const
    {
        return *_begin;
    }

    IntegerList::Iterator::Iterator(const vector_iterator& begin, const vector_iterator& end) : _begin(begin), _end(end)
    {}

    IntegerList::IntegerList()
    {}

    uint64 IntegerList::Size() const
    {
        return _list.size();
    }

    void IntegerList::Reserve(uint64 size)
    {
        _list.reserve(size);
    }

    uint64 IntegerList::Max() const
    {
        if(_list.size()==0)
        {
            throw std::runtime_error("Can't get max of empty list");
        }

        return _list[Size()-1];
    }

    void IntegerList::PushBack(uint64 value)
    {
        _list.push_back(value);
    }

    void IntegerList::Reset()
    {
        _list.resize(0);
    }

    IntegerList::Iterator IntegerList::GetIterator() const
    {
        return Iterator(_list.cbegin(), _list.cend());
    }
}
