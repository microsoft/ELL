////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UniqueId.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueId.h"

namespace ell
{
namespace utilities
{
    size_t UniqueId::_nextId = 1000;

    UniqueId::UniqueId()
    {
        _id = std::to_string(_nextId);
        ++_nextId;
    }

    UniqueId::UniqueId(const std::string& idString)
    {
        _id = idString;
    }

    bool UniqueId::operator==(const UniqueId& other) const
    {
        return _id == other._id;
    }

    bool UniqueId::operator!=(const UniqueId& other) const
    {
        return !(other == *this);
    }

    bool UniqueId::operator<(const UniqueId& other) const
    {
        return _id < other._id;
    }

    bool UniqueId::operator>(const UniqueId& other) const
    {
        return _id > other._id;
    }

    std::ostream& operator<<(std::ostream& stream, const UniqueId& id)
    {
        stream << id._id;
        return stream;
    }

    void UniqueId::WriteToArchive(Archiver& archiver) const
    {
        archiver << to_string(*this);
        // archiver["id"] << to_string(*this);
    }

    void UniqueId::ReadFromArchive(Unarchiver& archiver)
    {
        std::string idString;
        // archiver["id"] >> idString;
        archiver >> idString;
        *this = UniqueId(idString);
    }

    std::string to_string(const UniqueId& id)
    {
        return id._id;
    }
}
}

std::hash<ell::utilities::UniqueId>::result_type std::hash<ell::utilities::UniqueId>::operator()(argument_type const& id) const
{
    return std::hash<std::string>()(id._id);
}
