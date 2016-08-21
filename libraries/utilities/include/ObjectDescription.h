////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"

// stl
#include <unordered_map>
#include <string>

/// <summary> utilities namespace </summary>
namespace utilities
{
    /// <summary> Holds information describing a property of an object </summary>
    class PropertyDescription
    {
    public:
        template <typename ValueType>
        PropertyDescription();

    private:
        std::string _typeName;
        std::string _description;
        Variant _value;
    };

    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectDescription
    {
    public:
        ObjectDescription() = default;

        template <typename ValueType>
        void AddProperty(std::string name, std::string description);

    private:
        std::string _typeName; // ???
        std::unordered_map<std::string, PropertyDescription> _properties;
    };

    /// <summary> Base class for describable objects </summary>
    class IDescribable
    {
    public:
        virtual ObjectDescription GetDescription() const = 0;
    };
}
