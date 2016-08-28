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

        /// <summary> Gets the documentation string for this object, if present </summary>
        std::string GetDocumentation() const { return _documentation; }

        /// <summary> Gets the string representing the type name of this object </summary>
        std::string GetObjectTypeName() const { return _typeName; }

        /// <summary> Checks if object has a property of a given name </summary>
        bool HasProperty(const std::string& name) const;

        /// <summary> Gets the properties of this object </summary>
        const PropertyCollection& Properties() const { return _properties; } // TODO: dynamically add the properties for the children if necessary

        /// <summary> Adds a new property to the object </summary>
        template <typename ValueType>
        void AddProperty(const std::string& name, std::string documentation);

        template <typename ValueType>
        ValueType GetPropertyValue(const std::string& name) const; // necessary?

        template <typename ValueType>
        void SetPropertyValue(const std::string& name, const ValueType& value);

        /// <summary> Retrieves an object property given its name </summary>
        ObjectDescription& operator[](const std::string& propertyName);        

        /// <summary> Tells if the object description has a value associated with it. </summary>
        ///
        /// <returns> `true` if the object description has a value, `false` otherwise. </returns>
        bool HasValue() const { return !_value.IsEmpty(); }

        /// <summary> Gets the value of this object. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the return value. </typeparam>
        /// <returns> The value of this object. </returns>
        template <typename ValueType>
        ValueType GetValue() const { return _value.GetValue<ValueType>(); }

        /// <summary> Gets the value of this object as a string </summary>
        ///
        /// <returns> The value of this object as a string </returns>
        std::string GetValueString() const;

        /// <summary> Sets the value of an object </summary>
        template <typename ValueType>
        void SetValue(ValueType&& value);

        /// <summary> Sets the value of an object </summary>
        template <typename ValueType>
        void operator=(ValueType&& value);

        void FillInDescription();

    private:
        std::string _typeName;
        std::string _documentation;
        std::unordered_map<std::string, ObjectDescription> _properties; // empty for non-describable objects

        // _value is empty (? or should it still be the object) if this object is an IDescribable, or a type description
        Variant _value; // null if this is an IDescribable or a type description
        // something to get description from type (a std::function, perhaps --- if empty, we're a non-describable thing)
        std::function<ObjectDescription()> _getPropertiesFunction; 

        template <typename ValueType>
        friend ObjectDescription MakeObjectDescription(const std::string& description);

        ObjectDescription(const std::string& documentation);

        template <typename ValueType>
        void SetGetPropertiesFunction(std::true_type);

        template <typename ValueType>
        void SetGetPropertiesFunction(std::false_type);
    };

    /// <summary> Base class for describable objects </summary>
    class IDescribable
    {
    public:
        virtual ObjectDescription GetDescription() const = 0;
        static std::string GetTypeName() {return "IDescribable"; }
    };

    /// <summary> Factory method to create an ObjectDescription </summary>
    template <typename ValueType>
    ObjectDescription MakeObjectDescription(const std::string& description);
}

#include "../tcc/ObjectDescription.tcc"
