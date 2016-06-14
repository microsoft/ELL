////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueId.h"

/// <summary> model namespace </summary>
namespace model
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

std::hash<model::UniqueId>::result_type std::hash<model::UniqueId>::operator()(argument_type const& id) const
{
    return std::hash<size_t>()(id._id);
}
