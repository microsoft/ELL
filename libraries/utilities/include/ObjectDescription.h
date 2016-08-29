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
        // iterator? iterator deref calls fillin function
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
        ///
        /// <returns> The documentation string for this object, if present </returns>
        std::string GetDocumentation() const { return _documentation; }

        /// <summary> Gets the string representing the type name of this object </summary>
        ///
        /// <returns> The string representing the type name of this object </returns>
        std::string GetObjectTypeName() const { return _typeName; }

        /// <summary> Checks if object has a property of a given name </summary>
        ///
        /// <param name="name"> The name of the property </param>
        /// <returns> `true` if the object has a property of the given name, `false` otherwise </returns>
        bool HasProperty(const std::string& name) const;

        /// <summary> Gets the properties of this object </summary>
        ///
        /// <returns> The properties of this object </returns>
        const PropertyCollection& Properties() const;

        /// <summary> Adds a new property to the object </summary>
        ///
        /// <typeparam name="ValueType"> The type this property's value will take </typeparam>
        /// <param name="name"> The name of the property </param>
        /// <param name="documentation"> The documentation string from the property </param>        
        template <typename ValueType>
        void AddProperty(const std::string& name, std::string documentation);

        /// <summary> Retrieves an object property given its name </summary>
        ///
        /// <param name="propertyName"> The name of the property to retrieve </param>
        /// <returns> The property's description </returns>
        const ObjectDescription& operator[](const std::string& propertyName) const;        

        /// <summary> Retrieves an object property given its name </summary>
        ///
        /// <param name="propertyName"> The name of the property to retrieve </param>
        /// <returns> The property's description </returns>
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
        ///
        /// <typeparam name="ValueType"> The type of the value to set </typeparam>
        /// <param name="value"> The value to set the parameter to </param>
        template <typename ValueType>
        void SetValue(ValueType&& value);

        /// <summary> Sets the value of an object </summary>
        ///
        /// <typeparam name="ValueType"> The type of the value to set </typeparam>
        /// <param name="value"> The value to set the parameter to </param>
        template <typename ValueType>
        void operator=(ValueType&& value);

    private:
        std::string _typeName;
        std::string _documentation;
        mutable std::unordered_map<std::string, ObjectDescription> _properties;

        Variant _value;

        mutable std::function<ObjectDescription()> _getPropertiesFunction; 

        template <typename ValueType>
        friend ObjectDescription MakeObjectDescription(const std::string& description);

        ObjectDescription(const std::string& documentation);

        // overload that gets called for IDescribable properties
        template <typename ValueType>
        void SetGetPropertiesFunction(std::true_type);

        // overload that gets called for non-IDescribable properties
        template <typename ValueType>
        void SetGetPropertiesFunction(std::false_type);

        void FillInDescription() const;
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
