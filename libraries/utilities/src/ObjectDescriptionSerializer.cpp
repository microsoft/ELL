////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescriptionSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescriptionSerializer.h"
#include "Serializer.h"
#include "ISerializable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cassert>

namespace utilities
{
    //
    // Serialization
    //
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionSerializer, double);

    // strings
    void ObjectDescriptionSerializer::SerializeValue(const char* name, const std::string& value) 
    {
        if(std::string{""} == name)
        {
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // ISerializable

    void ObjectDescriptionSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        value.Serialize(*this);
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionSerializer, double);

    void ObjectDescriptionSerializer::SerializeArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void ObjectDescriptionSerializer::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
    {
        if(std::string{""} == name)
        {
            _objectDescription << array;
        }
        else
        {
            _objectDescription[name] << array;
        }
    }

    //
    // Deserialization
    //
    ObjectDescriptionDeserializer::ObjectDescriptionDeserializer(const ObjectDescription& objectDescription, SerializationContext context) : Deserializer(std::move(context)), _objectDescription(objectDescription) {}

    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionDeserializer, double);

    // strings
    void ObjectDescriptionDeserializer::DeserializeValue(const char* name, std::string& value) 
    { 
        if(std::string{""} == name)
        {
            _objectDescription >> value;
        }
        else
        {
            _objectDescription[name] >> value;
        }
    }

    // ISerializable
    void ObjectDescriptionDeserializer::DeserializeObject(const char* name, ISerializable& value) 
    {
        value.Deserialize(*this);
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionDeserializer, double);

    void ObjectDescriptionDeserializer::DeserializeArray(const char* name, std::vector<std::string>& array)
    {
        if(std::string{""} == name)
        {
            _objectDescription >> array;
        }
        else
        {
            _objectDescription[name] >> array;
        }
    }

    bool ObjectDescriptionDeserializer::BeginDeserializeArrayItem(const std::string& typeName)
    {
        return true;
    }

    void ObjectDescriptionDeserializer::EndDeserializeArrayItem(const std::string& typeName)
    {        
    }


}
