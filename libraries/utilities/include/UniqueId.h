////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <ostream>

/// <summary> model namespace </summary>
namespace utilities
{
    /// <summary> UniqueId: A placeholder for a real GUID-type class </summary>
    class UniqueId
    {
    public:
        /// <summary> Constructor </summary>
        UniqueId();
        UniqueId(const UniqueId& other) = default;
        UniqueId& operator=(const UniqueId& other) = default;

        /// <summary> Equality comparison </summary>
        bool operator==(const UniqueId& other) const;

        /// <summary> Inequality comparison </summary>
        bool operator!=(const UniqueId& other) const;

        /// <summary> Stream output </summary>
        friend std::ostream& operator<<(std::ostream& stream, const UniqueId& id)
        {
            stream << id._id;
            return stream;
        }

    private:
        friend std::hash<UniqueId>;
        size_t _id;
        static size_t _nextId;
    };
}

// custom specialization of std::hash so we can keep UniqueIds in containers that require hashable types
namespace std
{
    template <>
    class hash<utilities::UniqueId>
    {
    public:
        typedef utilities::UniqueId argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& id) const;
    };
}
