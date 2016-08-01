////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UniqueId.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ISerializable.h"

#include <functional>
#include <ostream>
#include <string>

/// <summary> model namespace </summary>
namespace utilities
{
    /// <summary> UniqueId: A placeholder for a real GUID-type class </summary>
    class UniqueId : public ISerializable
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

        /// Inherited from ISerializable
        virtual void Serialize(utilities::Serializer& serializer) const override;
        virtual void Deserialize(utilities::Deserializer& serializer, SerializationContext& context) override;
        
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
