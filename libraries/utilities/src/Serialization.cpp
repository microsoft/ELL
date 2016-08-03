////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serialization.h"
#include "Format.h"
#include "ISerializable.h"

#include <iostream>
#include <string>

namespace utilities
{
    //
    // Serialization
    //
    void Serializer::SerializeValue(const char* name, const ISerializable& value)
    {
        BeginSerializeObject(name, value);
        SerializeObject(name, value);
        EndSerializeObject(name, value);
    }

    void Serializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        // nothing
    }

    void Serializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
        // nothing
    }

    //
    // Deserialization
    //
    void Deserializer::DeserializeValue(const char* name, ISerializable& value, SerializationContext& context)
    {
        auto typeName = BeginDeserializeObject(name, value, context);
        DeserializeObject(name, value, context);
        EndDeserializeObject(name, value, context);
    }

    std::string Deserializer::BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context)
    {
        return "";
    }

    void Deserializer::EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context)
    {
        // nothing
    }
}
