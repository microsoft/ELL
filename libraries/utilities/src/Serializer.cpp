////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serializer.h"
#include "Format.h"
#include "ISerializable.h"

#include <string>

namespace utilities
{
    //
    // Serializer
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
    // Deserializer
    //
    Deserializer::Deserializer(SerializationContext context) : _baseContext(context)
    {        
        _contexts.push_back(_baseContext);
    }

    void Deserializer::PushContext(SerializationContext& context)
    {
        _contexts.push_back(context);
    }
    
    void Deserializer::DeserializeValue(const char* name, ISerializable& value)
    {
        auto typeName = value.GetRuntimeTypeName();
        typeName = BeginDeserializeObject(name, "");
        DeserializeObject(name, value);
        EndDeserializeObject(name, typeName);
    }

    std::string Deserializer::BeginDeserializeObject(const char* name, const std::string& typeName)
    {
        return "";
    }

    void Deserializer::EndDeserializeObject(const char* name, const std::string& typeName)
    {
        // nothing
    }

    void Deserializer::BeginDeserializeArray(const char* name, const std::string& typeName)
    {
    }

    void Deserializer::EndDeserializeArray(const char* name, const std::string& typeName)
    {
    }
}
