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
#include <functional>
#include <iostream>
#include <type_traits>

/// <summary> utilities namespace </summary>
namespace utilities
{

    template <typename KeyType, typename ValueType>
    class OnDemandMap
    {
    public:
        // op[]
        // op[] const
        // add? -- with creator function
        // iterator?
    private:
        std::unordered_map<KeyType, ValueType> _values;
        std::unordered_map<KeyType, std::function<ValueType()>> _creators;
    };




    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectDescription
    {
    public:
        using PropertyCollection = std::unordered_map<std::string, ObjectDescription>;

        ObjectDescription() = default;

        ObjectDescription(const std::string& description) : _description(description) {};

        template <typename ValueType>
        static ObjectDescription MakeObjectDescription(const std::string& description);

        std::string GetDescription() const { return _description; }
        std::string GetObjectTypeName() const { return _typeName; }

        /// <summary> Adds a new property to the object </summary>
        template <typename ValueType>
        void AddProperty(const std::string& name, std::string description);

        /// <summary> Gets the properties of this object </summary>
        const PropertyCollection& Properties() const { return _properties; }

        /// <summary> Checks if object has a property of a given name </summary>
        bool HasProperty(const std::string& name) const;

        template <typename ValueType>
        ValueType GetPropertyValue(const std::string& name) const;

        template <typename ValueType>
        void SetPropertyValue(const std::string& name, const ValueType& value);

        /// <summary> Retrieves an object property given its name </summary>
        ObjectDescription& operator[](const std::string& propertyName);        

        /// <summary> Tells if the object description has a value associated with in </summary>
        bool HasValue() const { return _value.IsEmpty(); }

        bool IsDescribable() const;        

        /// <summary> Gets the value of an object </summary>
        template <typename ValueType>
        ValueType GetValue() const { return _value.GetValue<ValueType>(); }

        /// <summary> Sets the value of an object </summary>
        template <typename ValueType>
        void SetValue(ValueType&& value);

        /// <summary> Sets the value of an object </summary>
        template <typename ValueType>
        void operator=(ValueType&& value);

        void FillInDescription();

    private:
        std::string _typeName;
        std::string _description;
        std::unordered_map<std::string, ObjectDescription> _properties; // empty for non-describable objects

        // _value is empty (? or should it still be the object) if this object is an IDescribable, or a type description
        Variant _value; // null if this is an IDescribable or a type description
        // something to get description from type (a std::function, perhaps --- if empty, we're a non-describable thing)
        std::function<ObjectDescription()> _getPropertyDescription; 

        template <typename ValueType>
        void SetFillInDescriptionFunction(std::true_type);
        template <typename ValueType>
        void SetFillInDescriptionFunction(std::false_type);
    };

    /// <summary> Base class for describable objects </summary>
    class IDescribable
    {
    public:
        virtual ObjectDescription GetDescription() const = 0;
    };
}

#include "../tcc/ObjectDescription.tcc"