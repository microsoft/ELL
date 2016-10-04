////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueId.h"

#include <string>

namespace emll
{
/// <summary> utilities namespace </summary>
namespace utilities
{
    size_t UniqueId::_nextId = 1000;

    UniqueId::UniqueId()
    {
        _id = _nextId;
        ++_nextId;
    }

    UniqueId::UniqueId(const std::string& idString)
    {
        _id = std::stoi(idString);
        if(_nextId <= _id)
        {
            _nextId = _id+1;
        }
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
        archiver["id"] << to_string(*this);
    }

    void UniqueId::ReadFromArchive(Unarchiver& archiver)
    {
        std::string idString;
        archiver["id"] >> idString;
        *this = UniqueId(idString);
    }

    std::string to_string(const UniqueId& id)
    {
        using std::to_string;
        return to_string(id._id);
    }
}
}

std::hash<emll::utilities::UniqueId>::result_type std::hash<emll::utilities::UniqueId>::operator()(argument_type const& id) const
{
    return std::hash<size_t>()(id._id);
}
