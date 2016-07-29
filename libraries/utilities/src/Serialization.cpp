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
#include "Variant.h"

#include <iostream>
#include <string>

namespace utilities
{
#define IMPLEMENT_FUNDAMENTAL_SERIALIZE(base, type)     void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { SerializeScalar(name,value); }
#define IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(base, type)     void base::SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { SerializeArray(name,value); }

    //
    // Serializer base class
    //
    void Serializer::SerializeValue(const char* name, const ISerializable& value)
    {
        BeginSerializeObject(name, value);
        SerializeObject(name, value);
        EndSerializeObject(name, value);
    }

    void Serializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
    }

    void Serializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
    }

    //
    // Scalars
    //
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, bool);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, char);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, short);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, int);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, size_t);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, float);
    IMPLEMENT_FUNDAMENTAL_SERIALIZE(SimpleSerializer, double);
    
    // strings
    void SimpleSerializer::SerializeValue(const char* name, std::string value)
    {
        SerializeScalar(name, value);
    }

    // ISerializable

    void SimpleSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        if (name != std::string(""))
        {
            std::cout << name << ": ";
        }
        std::cout << "{" << std::endl;
        std::cout << "_type: " << value.GetRuntimeTypeName() << std::endl;
        value.Serialize(*this);
        std::cout << "}" << std::endl;
    }


    //
    // Arrays
    //
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, bool);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, char);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, short);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, int);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, size_t);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, float);
    IMPLEMENT_FUNDAMENTAL_ARRAY_SERIALIZE(SimpleSerializer, double);

    void SimpleSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
    {
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

    void SimpleSerializer::Deserialize(const char* name, ISerializable& value)
    {

    }
}
