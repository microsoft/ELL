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
        const PropertyCollection& GetProperties() const;

        /// <summary> Gets the properties of this object </summary>
        ///
        /// <returns> The properties of this object </returns>
        PropertyCollection& GetProperties();

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
        std::string _documentation;
        Variant _value;
        mutable std::unordered_map<std::string, ObjectDescription> _properties;
        mutable std::function<ObjectDescription(const ObjectDescription* self)> _fillInPropertiesFunction;

        // friends
        template <typename ValueType>
        friend ObjectDescription MakeObjectDescription(const std::string& description);
        template <typename BaseType, typename ValueType>
        friend ObjectDescription MakeObjectDescription(const std::string& description);
        friend class IDescribable;

        // internal private constructor
        ObjectDescription(const std::string& documentation);

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

        /// <summary> Gets an ObjectDescription for the object </summary>
        ///
        /// <returns> An ObjectDescription for the object </returns>
        virtual ObjectDescription GetDescription() const = 0;

        /// <summary> Sets the internal state of the object according to the description passed in </summary>
        ///
        /// <param name="description"> The `ObjectDescription` to get state from </param>
        virtual void SetObjectState(const ObjectDescription& description, SerializationContext& context) = 0;

        /// <summary> Utility function for polymorphic types to get parent's description as starting point </summary>
        template <typename BaseType, typename ValueType>
        ObjectDescription GetParentDescription() const;

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

    /// <summary> Factory method to create an ObjectDescription </summary>
    template <typename ValueType>
    ObjectDescription MakeObjectDescription(const std::string& description);

    template <typename BaseType, typename ValueType>
    ObjectDescription MakeObjectDescription(const std::string& description);

    /// <summary> Enabled if ValueType inherits from IDescribable. </summary>
    template <typename ValueType>
    using IsDescribable = typename std::enable_if_t<std::is_base_of<IDescribable, typename std::decay<ValueType>::type>::value, int>;

    /// <summary> Enabled if ValueType does not inherit from IDescribable. </summary>
    template <typename ValueType>
    using IsNotDescribable = typename std::enable_if_t<(!std::is_base_of<IDescribable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;
}

#include "../tcc/ObjectDescription.tcc"
