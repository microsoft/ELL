////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescriptionArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescriptionArchiver.h"
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
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, bool);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, char);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, short);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, int);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, size_t);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, float);
    IMPLEMENT_SERIALIZE_VALUE(ObjectDescriptionArchiver, double);

    // strings
    void ObjectDescriptionArchiver::SerializeValue(const char* name, const std::string& value) 
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

    void ObjectDescriptionArchiver::SerializeObject(const char* name, const ISerializable& value)
    {
//        _objectDescription.SetValue(value); // this doesn't work, because it's setting the type to ISerializable, which is abstract
        if(std::string{""} == name)
        {
            value.Serialize(*this);
        }
        else
        {
            // need to create new object description serializer here, I guess
            ObjectDescriptionArchiver archiver;
            archiver << value;
            _objectDescription[name] = archiver.GetObjectDescription();
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, bool);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, char);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, short);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, int);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, float);
    IMPLEMENT_SERIALIZE_ARRAY(ObjectDescriptionArchiver, double);

    void ObjectDescriptionArchiver::SerializeArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void ObjectDescriptionArchiver::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
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
    ObjectDescriptionUnarchiver::ObjectDescriptionUnarchiver(const ObjectDescription& objectDescription, SerializationContext context) : Deserializer(std::move(context)), _objectDescription(objectDescription) 
    {
        int x = 5;
    }

    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, bool);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, char);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, short);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, int);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, float);
    IMPLEMENT_DESERIALIZE_VALUE(ObjectDescriptionUnarchiver, double);

    // strings
    void ObjectDescriptionUnarchiver::DeserializeValue(const char* name, std::string& value) 
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
    void ObjectDescriptionUnarchiver::DeserializeObject(const char* name, ISerializable& value) 
    {
        value.Deserialize(*this);
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, char);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, short);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, int);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, float);
    IMPLEMENT_DESERIALIZE_ARRAY(ObjectDescriptionUnarchiver, double);

    void ObjectDescriptionUnarchiver::DeserializeArray(const char* name, std::vector<std::string>& array)
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

    bool ObjectDescriptionUnarchiver::BeginDeserializeArrayItem(const std::string& typeName)
    {
        return true;
    }

    void ObjectDescriptionUnarchiver::EndDeserializeArrayItem(const std::string& typeName)
    {        
    }


}
