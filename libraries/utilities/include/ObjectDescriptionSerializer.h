////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescriptionSerializer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Serializer.h"
#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"
#include "ObjectDescription.h"

// stl
#include <cstdint>
#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    /// <summary> A serializer that encodes data in an ObjectDescription </summary>
    class ObjectDescriptionSerializer : public Serializer
    {
    public:
        /// <summary> Default Constructor. </summary>
        ObjectDescriptionSerializer() = default;
        ObjectDescription GetObjectDescription();

    protected:
        DECLARE_SERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(double);
        virtual void SerializeValue(const char* name, const std::string& value) override;

        DECLARE_SERIALIZE_ARRAY_OVERRIDE(bool);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(char);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(short);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(int);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(size_t);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(float);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(double);
        virtual void SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array) override;

        virtual void SerializeObject(const char* name, const ISerializable& value) override;

    private:
        // Serialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        ObjectDescription _objectDescription;
    };

    /// <summary> A deserializer that reads data encoded in JSON-formatted text. </summary>
    class ObjectDescriptionDeserializer : public Deserializer
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="objectDescription"> The description to deserialize data from. </summary>
        ObjectDescriptionDeserializer(const ObjectDescription& objectDescription, SerializationContext context);

    protected:
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(double);
        virtual void DeserializeValue(const char* name, std::string& value) override;

        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(bool);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(char);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(short);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(int);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(float);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(double);
        virtual void DeserializeArray(const char* name, std::vector<std::string>& array) override;
        virtual void BeginDeserializeArray(const char* name, const std::string& typeName) override;
        virtual bool BeginDeserializeArrayItem(const std::string& typeName) override;
        virtual void EndDeserializeArrayItem(const std::string& typeName) override;
        virtual void EndDeserializeArray(const char* name, const std::string& typeName) override;

        virtual std::string BeginDeserializeObject(const char* name, const std::string& typeName) override;
        virtual void DeserializeObject(const char* name, ISerializable& value) override;
        virtual void EndDeserializeObject(const char* name, const std::string& typeName) override;

    private:
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);

        void ReadArray(const char* name, std::vector<std::string>& array);
        ObjectDescription _objectDescription;
    };

}

#include "../tcc/ObjectDescriptionSerializer.tcc"
