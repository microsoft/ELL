////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchive.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"
#include "Exception.h"
#include "ISerializable.h"

// stl
#include <unordered_map>
#include <string>
#include <cassert>
#include <functional>
#include <type_traits>

/// <summary> utilities namespace </summary>
namespace utilities
{
    // Temporary
    using Archiver = Archiver; // ObjectArchive;
    using Unarchiver = Unarchiver; // ObjectArchive;

    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectArchive
    {
    public:
        using PropertyCollection = std::unordered_map<std::string, ObjectArchive>;

        ObjectArchive() = default;

        /// <summary> Gets the string representing the type name of this object </summary>
        ///
        /// <returns> The string representing the type name of this object </returns>
        std::string GetObjectTypeName() const { return _typeName; }

        /// <summary> Sets the type of this object </summary>
        ///
        /// <typeparam name="ValueType"> The type of the object </typeparam>
        /// <param name="object"> The object whose type to use </param>
        template <typename ValueType>
        void SetType(const ValueType& object);

        /// <summary> Checks if object has a property of a given name </summary>
        ///
        /// <param name="name"> The name of the property </param>
        /// <returns> `true` if the object has a property of the given name, `false` otherwise </returns>
        bool HasProperty(const std::string& name) const;

        /// <summary> Gets the properties of this object </summary>
        ///
        /// <returns> The properties of this object </returns>
        const PropertyCollection& GetProperties() const;

        /// <summary> Gets the properties of this object </summary>
        ///
        /// <returns> The properties of this object </returns>
        PropertyCollection& GetProperties();

        /// <summary> Retrieves an object property given its name </summary>
        ///
        /// <param name="propertyName"> The name of the property to retrieve </param>
        /// <returns> The property's description </returns>
        const ObjectArchive& operator[](const std::string& propertyName) const;

        /// <summary> Retrieves an object property given its name </summary>
        ///
        /// <param name="propertyName"> The name of the property to retrieve </param>
        /// <returns> The property's description </returns>
        ObjectArchive& operator[](const std::string& propertyName);

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

        /// <summary> Copies this object's value to a variable. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the value value. </typeparam>
        /// <param name="value"> The variable to copy the value to. </param>
        template <typename ValueType>
        void CopyValueTo(ValueType&& value) const;

        /// <summary> Copies this object's value to a variable. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the value value. </typeparam>
        /// <param name="value"> The variable to copy the value to. </param>
        template <typename ValueType>
        void operator>>(ValueType&& value) const;

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
        void operator<<(ValueType&& value);


    private:
        std::string _typeName;
        Variant _value;
        mutable std::unordered_map<std::string, ObjectArchive> _properties;
        mutable std::function<ObjectArchive(const ObjectArchive* self)> _fillInPropertiesFunction;

        // friends
        friend class IDescribable;

        // overload that gets called for IDescribable properties
        template <typename ValueType>
        void SetGetPropertiesFunction(std::true_type);

        // overload that gets called for non-IDescribable properties
        template <typename ValueType>
        void SetGetPropertiesFunction(std::false_type);

        void FillInDescription() const;
    };
    
    /// <summary>
    /// IDescribable is the Base class for describable objects. In order to be able to use the
    /// IDescribable interface for serialization, you must also implement GetRuntimeTypeName, GetTypeName,
    /// and the static GetTypeDescription functions.
    /// </summary>
    class IDescribable : public ISerializable
    {
    public:
        virtual ~IDescribable() = default;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <returns> The ObjectArchive for the object </returns>
        ObjectArchive GetDescription() const;

        /// <summary> Creates an object from an `Archiver` </summary>
        ///
        /// <typeparam name="ValueType"> The type of the object to retrieve </typeparam>
        /// <param name="archiver"> The `Archiver` to get the object from </param>
        /// <returns> The new object </returns>
        template <typename ValueType>
        static ValueType CreateObject(const Archiver& archiver);

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IDescribable"; }
    };
}

#include "../tcc/ObjectArchive.tcc"
