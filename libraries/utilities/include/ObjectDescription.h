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
        static PropertyDescription MakePropertyDescription(const std::string& description);

        template <typename ValueType>
        static PropertyDescription MakePropertyDescription(const std::string& description, const ValueType& value);

        template <typename ValueType>
        ValueType GetValue() const { return _value.GetValue<ValueType>(); }

        template <typename ValueType>
        void SetValue(ValueType&& value);

        template <typename ValueType>
        void operator=(ValueType&& value);

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

        ObjectDescription(const std::string& description) : _description(description) {};

        template <typename ValueType>
        void AddProperty(const std::string& name, std::string description);

        bool HasProperty(const std::string& name) const;

        template <typename ValueType>
        ValueType GetPropertyValue(const std::string& name) const;

        template <typename ValueType>
        void SetPropertyValue(const std::string& name, const ValueType& value);

        PropertyDescription& operator[](const std::string& propertyName);        

    private:
        std::string _typeName;
        std::string _description;
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