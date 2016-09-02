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
    /// <summary> Holds information describing the properties (fields) of an object </summary>
    class ObjectDescription
    {
    public:
        using PropertyCollection = std::unordered_map<std::string, ObjectDescription>;

        ObjectDescription() = default;

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

        /// <summary> Sets the value of an object </summary>
        ///
        /// <typeparam name="ValueType"> The type of the value to set </typeparam>
        /// <param name="value"> The value to set the parameter to </param>
        template <typename ValueType>
        void operator=(ValueType&& value);

    private:
        std::string _typeName;
        Variant _value;
        mutable std::unordered_map<std::string, ObjectDescription> _properties;
        mutable std::function<ObjectDescription(const ObjectDescription* self)> _fillInPropertiesFunction;

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
    // Temporary
    using Archiver = ObjectDescription;

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
        /// <returns> The ObjectDescription for the object </returns>
        virtual ObjectDescription GetDescription() const;

        /// <summary> Adds an objects properties to an ObjectDescription </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void AddProperties(ObjectDescription& description) const = 0;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void SetObjectState(const Archiver& archiver, SerializationContext& context) = 0;

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IDescribable"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override = 0;

        /// <summary> Serializes the object. </summary>
        ///
        /// <param name="serializer">  The serializer. </param>
        virtual void Serialize(Serializer& serializer) const override;

        /// <summary> Deserializes the object. </summary>
        ///
        /// <param name="serializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(Deserializer& serializer) override;
    };

    /// <summary> Enabled if ValueType inherits from IDescribable. </summary>
    template <typename ValueType>
    using IsDescribable = typename std::enable_if_t<std::is_base_of<IDescribable, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType does not inherit from IDescribable. </summary>
    template <typename ValueType>
    using IsNotDescribable = typename std::enable_if_t<(!std::is_base_of<IDescribable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;
}

#include "../tcc/ObjectDescription.tcc"
