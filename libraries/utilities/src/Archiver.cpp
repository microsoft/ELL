////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Archiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archiver.h"
#include "Format.h"
#include "ISerializable.h"

#include <string>

namespace utilities
{
    //
    // PropertyArchiver class
    //
    Archiver::PropertyArchiver::PropertyArchiver(Archiver& archiver, const std::string& name) : _serializer(archiver), _propertyName(name)
    {};

    //
    // Archiver class
    //
    void Archiver::SerializeValue(const char* name, const ISerializable& value)
    {
        BeginSerializeObject(name, value);
        SerializeObject(name, value);
        EndSerializeObject(name, value);
    }

    void Archiver::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        // nothing
    }

    void Archiver::EndSerializeObject(const char* name, const ISerializable& value)
    {
        // nothing
    }

    //
    // PropertyArchiver class
    //
    Unarchiver::PropertyUnarchiver::PropertyUnarchiver(Unarchiver& deserializer, const std::string& name) : _deserializer(deserializer), _propertyName(name)
    {};

    //
    // Unarchiver class
    //
    Unarchiver::Unarchiver(SerializationContext context) : _baseContext(context)
    {        
        _contexts.push_back(_baseContext);
    }

    Unarchiver::PropertyUnarchiver Unarchiver::operator[](const std::string& name) 
    { 
        return PropertyUnarchiver{ *this, name }; 
    }

    void Unarchiver::PushContext(SerializationContext& context)
    {
        _contexts.push_back(context);
    }
    
    void Unarchiver::DeserializeValue(const char* name, ISerializable& value)
    {
        auto typeName = BeginDeserializeObject(name, value.GetRuntimeTypeName());
        DeserializeObject(name, value);
        EndDeserializeObject(name, typeName);
    }

    std::string Unarchiver::BeginDeserializeObject(const char* name, const std::string& typeName)
    {
        return typeName;
    }

    void Unarchiver::EndDeserializeObject(const char* name, const std::string& typeName)
    {
    }

    void Unarchiver::BeginDeserializeArray(const char* name, const std::string& typeName)
    {
    }

    void Unarchiver::EndDeserializeArray(const char* name, const std::string& typeName)
    {
    }
}
