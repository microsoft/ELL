////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.h (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: move to utilities (?)

#pragma once

#include <functional>
#include <ostream>

/// <summary> UniqueId: A placeholder for a real GUID-type class </summary>
class UniqueId
{
public:    
    /// <summary> Constructor </summary>
    UniqueId();
    UniqueId(const UniqueId& other) = default;
    UniqueId& operator=(const UniqueId& other) = default;

    bool operator==(const UniqueId& other) const;
    bool operator!=(const UniqueId& other) const;

    friend std::ostream& operator<<(std::ostream& stream, const UniqueId& id) { stream << id._id; return stream; }
    
private:
    friend std::hash<UniqueId>;
    size_t _id;
    static size_t _nextId;
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<> class hash<UniqueId>
    {
    public:
        typedef UniqueId argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& id) const;
    };
}
