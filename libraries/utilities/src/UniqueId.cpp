////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueId.h"

/// <summary> utilities namespace </summary>
namespace utilities
{
    size_t UniqueId::_nextId = 1000;

    UniqueId::UniqueId()
    {
        _id = _nextId;
        ++_nextId;
    }

    bool UniqueId::operator==(const UniqueId& other) const { return _id == other._id; }

    bool UniqueId::operator!=(const UniqueId& other) const { return !(other == *this); }
}

std::hash<utilities::UniqueId>::result_type std::hash<utilities::UniqueId>::operator()(argument_type const& id) const
{
    return std::hash<size_t>()(id._id);
}
