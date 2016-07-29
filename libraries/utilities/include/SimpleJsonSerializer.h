////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.h (utilities)
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
    class SimpleJsonSerializer : public Serializer
    {
    public:
    protected:
        // virtual void SerializeVariant(std::string name, const Variant& variant) override;

        DECLARE_SERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(double);
        virtual void SerializeValue(const char* name, std::string value) override;

        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(bool);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(char);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(short);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(int);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(size_t);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(float);
        DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(double);
        virtual void SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array) override;

        virtual void BeginSerializeObject(const char* name, const ISerializable& value) override;
        virtual void SerializeObject(const char* name, const ISerializable& value) override;
        virtual void EndSerializeObject(const char* name, const ISerializable& value) override;

        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Serialize(const char* name, const ValueType& value);

    private:
        // Serialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        int _indent = 0;
        std::string GetCurrentIndent() { return std::string(2 * _indent, ' '); }
    };

    class SimpleJsonDeserializer : public Deserializer
    {
    public:
    protected:
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(double);
        virtual void DeserializeValue(const char* name, std::string& value) override;

        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(bool);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(char);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(short);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(int);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(float);
        DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(double);
        virtual void DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array) override;

        virtual void BeginDeserializeObject(const char* name, ISerializable& value) override;
        virtual void DeserializeObject(const char* name, ISerializable& value) override;
        virtual void EndDeserializeObject(const char* name, ISerializable& value) override;

    private:
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);
    };
}

#include "../tcc/SimpleJsonSerializer.tcc"
