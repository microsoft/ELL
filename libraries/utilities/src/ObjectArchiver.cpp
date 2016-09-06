////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectArchiver.h"
#include "Archiver.h"
#include "ISerializable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cassert>

namespace utilities
{
    ObjectArchiver::ObjectArchiver(SerializationContext context) : Unarchiver(context) 
    {        
    }

    ObjectArchiver::ObjectArchiver(const ObjectArchive& objectDescription, SerializationContext context) : Unarchiver(std::move(context)), _objectDescription(objectDescription) 
    {
    }

    //
    // Serialization
    //
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, bool);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, char);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, short);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, int);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, size_t);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, float);
    IMPLEMENT_SERIALIZE_VALUE(ObjectArchiver, double);

    // strings
    void ObjectArchiver::SerializeValue(const char* name, const std::string& value) 
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

    void ObjectArchiver::SerializeObject(const char* name, const ISerializable& value)
    {
        if(std::string{""} == name)
        {
            value.Serialize(*this);
        }
        else
        {
            // need to create new object description archiver here, I guess
            SerializationContext context;
            ObjectArchiver archiver(context);
            archiver << value;
            _objectDescription[name] = archiver.GetObjectArchive();
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, bool);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, char);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, short);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, int);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, float);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectArchiver, double);

    void ObjectArchiver::SerializeArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void ObjectArchiver::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
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
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, bool);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, char);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, short);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, int);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, float);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectArchiver, double);

    // strings
    void ObjectArchiver::DeserializeValue(const char* name, std::string& value) 
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
    void ObjectArchiver::DeserializeObject(const char* name, ISerializable& value) 
    {
        if (std::string{ "" } == name)
        {
            value.Deserialize(*this);
        }
        else
        {
            ObjectArchiver propertyUnarchiver(_objectDescription[name], GetContext());
            value.Deserialize(propertyUnarchiver);
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, char);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, short);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, int);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, float);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectArchiver, double);

    void ObjectArchiver::DeserializeArray(const char* name, std::vector<std::string>& array)
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

    bool ObjectArchiver::BeginDeserializeArrayItem(const std::string& typeName)
    {
        return true;
    }

    void ObjectArchiver::EndDeserializeArrayItem(const std::string& typeName)
    {        
    }


}
