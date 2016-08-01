////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"

// stl
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    class SerializationContext
    {
    public:
        virtual ~SerializationContext() = default;
    };

    // TODO: put all this stuff somewhere else
    template <typename ValueType>
    class is_vector
    {
        template <typename VectorType>
        static constexpr bool is_vector_checker(typename VectorType::value_type*, typename std::enable_if_t<std::is_base_of<VectorType, typename std::vector<typename VectorType::value_type>>::value, int> = 0)
        {
            return true;
        }

        template <typename VectorType>
        static constexpr bool is_vector_checker(...)
        {
            return false;
        }

    public:
        static const bool value = is_vector_checker<ValueType>(0);
    };

    class ISerializable;

    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotFundamental = typename std::enable_if_t<!std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, int>;

    template <typename ValueType>
    using IsSerializable = typename std::enable_if_t<std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotSerializable = typename std::enable_if_t<(!std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;

    template <typename ValueType>
    using IsVector = typename std::enable_if_t<is_vector<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotVector = typename std::enable_if_t<!is_vector<typename std::decay<ValueType>::type>::value, int>;

#define DECLARE_SERIALIZE_VALUE_BASE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_SERIALIZE_ARRAY_VALUE_BASE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_SERIALIZE_VALUE_OVERRIDE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) override;
#define DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    // TODO: replace all these const char* name entries with std::string
    class Serializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Serialize(ValueType&& value);

        // Maybe add SerializeField?

        // TODO: call these something else, and just call them from the generic Serialize(name, ValueType&& value)
        /// <summary> Serialize named values of any serializable type. </summary>
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Serialize(const char* name, ValueType&& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<const ValueType*>& value);

    protected:
        // TODO: Instead of having all these different overloads, we could possibly have SerializeVariant(const Variant& value) be the
        //       virtual interface, and let the subclass deal with extracting the value from the variant

        // virtual void SerializeVariant(std::string name, const Variant& variant) = 0;

        // fundamental types
        // TODO: maybe only require bool, char, int, and have rest use those?
        // Generates "SerializeValue" function definitions

        // These are all the virtual function that need to be implemented by serializers

        DECLARE_SERIALIZE_VALUE_BASE(bool);
        DECLARE_SERIALIZE_VALUE_BASE(char);
        DECLARE_SERIALIZE_VALUE_BASE(short);
        DECLARE_SERIALIZE_VALUE_BASE(int);
        DECLARE_SERIALIZE_VALUE_BASE(size_t);
        DECLARE_SERIALIZE_VALUE_BASE(float);
        DECLARE_SERIALIZE_VALUE_BASE(double);
        virtual void SerializeValue(const char* name, const char* value) = 0;
        virtual void SerializeValue(const char* name, const std::string& value) = 0;
        virtual void SerializeValue(const char* name, const ISerializable& value);

        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(bool);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(char);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(short);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(int);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(size_t);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(float);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(double);
        virtual void SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array) = 0;

        virtual void BeginSerializeObject(const char* name, const ISerializable& value);
        virtual void SerializeObject(const char* name, const ISerializable& value) = 0;
        virtual void EndSerializeObject(const char* name, const ISerializable& value);
    };

#define DECLARE_DESERIALIZE_VALUE_BASE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(type) virtual void DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_VALUE_OVERRIDE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;
#define DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(type) virtual void DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;
    class Deserializer
    {
    public:
        // TODO: add context
        template <typename ValueType>
        void Deserialize(ValueType&& value, SerializationContext& context);

        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType&& value, SerializationContext& context);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<const ValueType*>& value, SerializationContext& context);

    protected:
        DECLARE_DESERIALIZE_VALUE_BASE(bool);
        DECLARE_DESERIALIZE_VALUE_BASE(char);
        DECLARE_DESERIALIZE_VALUE_BASE(short);
        DECLARE_DESERIALIZE_VALUE_BASE(int);
        DECLARE_DESERIALIZE_VALUE_BASE(size_t);
        DECLARE_DESERIALIZE_VALUE_BASE(float);
        DECLARE_DESERIALIZE_VALUE_BASE(double);
        virtual void DeserializeValue(const char* name, std::string& value, SerializationContext& context) = 0;
        virtual void DeserializeValue(const char* name, ISerializable& value, SerializationContext& context);

        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(bool);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(char);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(short);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(int);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(size_t);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(float);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(double);
        virtual void DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array, SerializationContext& context) = 0;

        virtual std::string BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context); // returns typename
        virtual void DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) = 0;
        virtual void EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context);
    };
}

#define IMPLEMENT_SERIALIZE_VALUE(base, type)          void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name, value); }
#define IMPLEMENT_SERIALIZE_ARRAY_VALUE(base, type)    void base::SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name, value); }

#define IMPLEMENT_DESERIALIZE_VALUE(base, type)        void base::DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy) { ReadScalar(name, value); }
#define IMPLEMENT_DESERIALIZE_ARRAY_VALUE(base, type)  void base::DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy) { ReadArray(name, value, context); }

#include "../tcc/Serialization.tcc"
