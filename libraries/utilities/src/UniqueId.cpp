////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueId.h"

#include <string>

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

    std::ostream& operator<<(std::ostream& stream, const UniqueId& id)
    {
        stream << id._id;
        return stream;
    }

    void UniqueId::Serialize(Archiver& archiver) const
    {
        archiver["id"] << _id;
    }

    void UniqueId::Deserialize(Unarchiver& archiver)
    {
        archiver["id"] >> _id;
    }

    std::string to_string(const UniqueId& id)
    {
        using std::to_string;
        return to_string(id._id);
    }
}

std::hash<utilities::UniqueId>::result_type std::hash<utilities::UniqueId>::operator()(argument_type const& id) const
{
    return std::hash<size_t>()(id._id);
}
