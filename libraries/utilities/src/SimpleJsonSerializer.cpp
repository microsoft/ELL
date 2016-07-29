////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serialization.h"
#include "SimpleJsonSerializer.h"
#include "Format.h"
#include "ISerializable.h"
#include "Variant.h"

#include <iostream>
#include <string>

namespace utilities
{
#define IMPLEMENT_SERIALIZE_VALUE(base, type)     void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name,value); }
#define IMPLEMENT_SERIALIZE_ARRAY_VALUE(base, type)     void base::SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name,value); }

#define IMPLEMENT_DESERIALIZE_VALUE(base, type)     void base::DeserializeValue(const char* name, type& value, IsFundamental<type> dummy) { ReadScalar(name,value); }
#define IMPLEMENT_DESERIALIZE_ARRAY_VALUE(base, type)     void base::DeserializeArrayValue(const char* name, std::vector<type>& value, IsFundamental<type> dummy) { ReadArray(name,value); }

    //
    // Serialization
    //
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, double);
    
    // strings
    void SimpleJsonSerializer::SerializeValue(const char* name, std::string value)
    {
        WriteScalar(name, value);
    }

    // ISerializable
    void SimpleJsonSerializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            std::cout << indent << name << ": ";
        }
        std::cout << "{" << std::endl;
        std::cout << indent << "_type: " << value.GetRuntimeTypeName() << std::endl;
    }

    void SimpleJsonSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void SimpleJsonSerializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        std::cout << indent << "}" << std::endl;
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, double);

    void SimpleJsonSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
    {
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            std::cout << name << ": ";
        }

        std::cout << "[";
        for(const auto& item: array)
        {
            Serialize(*item);
            std::cout << ", ";
        }
        std::cout << "]";
    }

    //
    // Deserialization
    //
    //
    // Serialization
    //
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, double);
    
    // strings
    void SimpleJsonDeserializer::DeserializeValue(const char* name, std::string& value)
    {
    }

    // ISerializable
    void SimpleJsonDeserializer::BeginDeserializeObject(const char* name, ISerializable& value)
    {
    }

    void SimpleJsonDeserializer::DeserializeObject(const char* name, ISerializable& value)
    {
    }

    void SimpleJsonDeserializer::EndDeserializeObject(const char* name, ISerializable& value)
    {
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, double);

    void SimpleJsonDeserializer::DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array)
    {
    }
}
