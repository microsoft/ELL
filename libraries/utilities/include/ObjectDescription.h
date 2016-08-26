////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"
#include "Exception.h"

// stl
#include <unordered_map>
#include <string>
#include <cassert>

/// <summary> utilities namespace </summary>
namespace utilities
{
    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectDescription
    {
    public:
        ObjectDescription() = default;

        ObjectDescription(const std::string& description) : _description(description) {};

        template <typename ValueType>
        static ObjectDescription MakeObjectDescription(const std::string& description);

        template <typename ValueType>
        void AddProperty(const std::string& name, std::string description);

        bool HasProperty(const std::string& name) const;

        std::string GetDescription() const { return _description; }
        std::string GetObjectTypeName() const { return _typeName; }

        using PropertyCollection = std::unordered_map<std::string, ObjectDescription>;
        const PropertyCollection& Properties() const { return _properties; }

        template <typename ValueType>
        ValueType GetPropertyValue(const std::string& name) const;

        template <typename ValueType>
        void SetPropertyValue(const std::string& name, const ValueType& value);

        ObjectDescription& operator[](const std::string& propertyName);        

        template <typename ValueType>
        ValueType GetValue() const { return _value.GetValue<ValueType>(); }

        template <typename ValueType>
        void SetValue(ValueType&& value);

        template <typename ValueType>
        void operator=(ValueType&& value);

    private:
        std::string _typeName;
        std::string _description;
        std::unordered_map<std::string, ObjectDescription> _properties; // empty for non-describable objects

        // for unifying ObjectDescription and PropertyDescription...
        Variant _value; // null if this is an IDescribable or a type description
        // something to get description from type (a std::function, perhaps --- if empty, we're a non-describable thing)

    };

    /// <summary> Base class for describable objects </summary>
    class IDescribable
    {
    public:
        virtual ObjectDescription GetDescription() const = 0;
    };
}

#include "../tcc/ObjectDescription.tcc"