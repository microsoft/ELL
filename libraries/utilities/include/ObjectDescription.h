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
    /// <summary> Holds information describing a property of an object </summary>
    class PropertyDescription
    {
    public:
        PropertyDescription();

        template <typename ValueType>
        PropertyDescription(const std::string& description, const ValueType& value);

        template <typename ValueType>
        ValueType GetValue() const { return _value.GetValue<ValueType>(); }

    private:
        std::string _typeName;
        std::string _description;
        Variant _value;
    };

    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectDescription
    {
    public:
        // TODO: templated constructor that sets typename?
        ObjectDescription() = default;

        template <typename ValueType>
        void AddProperty(const std::string& name, std::string description);

        template <typename ValueType>
        void AddProperty(const std::string& name, std::string description, const ValueType& value);

        bool HasProperty(const std::string& name) const;

        template <typename ValueType>
        ValueType GetPropertyValue(const std::string& name) const;

    private:
        std::string _typeName; // ???
        std::string description;
        std::unordered_map<std::string, PropertyDescription> _properties;
    };

    /// <summary> Base class for describable objects </summary>
    class IDescribable
    {
    public:
        virtual ObjectDescription GetDescription() const = 0;
    };
}

#include "../tcc/ObjectDescription.tcc"