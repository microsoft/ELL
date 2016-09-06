////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchiveSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchiveSerializer.h"
#include "Serializer.h"
#include "ISerializable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cassert>

namespace utilities
{
    ObjectArchiveSerializer::ObjectArchiveSerializer(SerializationContext context) : Deserializer(context) 
    {        
    }

    ObjectArchiveSerializer::ObjectArchiveSerializer(const ObjectArchive& objectDescription, SerializationContext context) : Deserializer(std::move(context)), _objectDescription(objectDescription) 
    {
    }

    //
    // Serialization
    //
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiveSerializer, double);

    // strings
    void ObjectArchiveSerializer::SerializeValue(const char* name, const std::string& value) 
    {
        if(std::string{""} == name)
        {
            _objectDescription.SetValue(value);
            _objectDescription << value;
        }
        else
        {
            _objectDescription[name] << value;
        }
    }

    // ISerializable

    void ObjectArchiveSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        if(std::string{""} == name)
        {
            value.Serialize(*this);
        }
        else
        {
            // need to create new object description serializer here, I guess
            SerializationContext context;
            ObjectArchiveSerializer archiver(context);
            archiver << value;
            _objectDescription[name] = archiver.GetObjectArchive();
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiveSerializer, double);

    void ObjectArchiveSerializer::SerializeArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void ObjectArchiveSerializer::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
    {
        if(std::string{""} == name)
        {
            _objectDescription.SetValue(array);
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
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiveSerializer, double);

    // strings
    void ObjectArchiveSerializer::DeserializeValue(const char* name, std::string& value) 
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
    void ObjectArchiveSerializer::DeserializeObject(const char* name, ISerializable& value) 
    {
        if (std::string{ "" } == name)
        {
            value.Deserialize(*this);
        }
        else
        {
            ObjectArchiveSerializer propertyUnarchiver(_objectDescription[name], GetContext());
            value.Deserialize(propertyUnarchiver);
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiveSerializer, double);

    void ObjectArchiveSerializer::DeserializeArray(const char* name, std::vector<std::string>& array)
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

    bool ObjectArchiveSerializer::BeginDeserializeArrayItem(const std::string& typeName)
    {
        return true;
    }

    void ObjectArchiveSerializer::EndDeserializeArrayItem(const std::string& typeName)
    {        
    }


}
