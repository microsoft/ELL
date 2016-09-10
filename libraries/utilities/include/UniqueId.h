////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IArchivable.h"

#include <functional>
#include <ostream>
#include <string>

namespace emll
{
/// <summary> model namespace </summary>
namespace utilities
{
    /// <summary> UniqueId: A placeholder for a real GUID-type class </summary>
    class UniqueId : public IArchivable
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
        friend std::ostream& operator<<(std::ostream& stream, const UniqueId& id);
        friend std::string to_string(const UniqueId& id);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "UniqueId"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(Unarchiver& archiver) override;

    private:
        friend std::hash<UniqueId>;
        size_t _id = 0;
        static size_t _nextId;
    };
}
}

// custom specialization of std::hash so we can keep UniqueIds in containers that require hashable types
namespace std
{
/// <summary> Implements a hash function for the UniqueId class, so that it can be used with associative containers (maps, sets, and the like). </summary>
template <>
class hash<emll::utilities::UniqueId>
{
public:
    typedef emll::utilities::UniqueId argument_type;
    typedef std::size_t result_type;

    /// <summary> Computes a hash of the input value. </summary>
    ///
    /// <returns> A hash value for the given input. </returns>
    result_type operator()(argument_type const& id) const;
};
}
