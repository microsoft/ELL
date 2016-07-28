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

#define IMPLEMENT_SERIALIZE(base, type)     void base::SerializeValue(const char* name, type value) { SerializeFundamental(name,value); }

    IMPLEMENT_SERIALIZE(SimpleSerializer, bool);
    IMPLEMENT_SERIALIZE(SimpleSerializer, char);
    IMPLEMENT_SERIALIZE(SimpleSerializer, short);
    IMPLEMENT_SERIALIZE(SimpleSerializer, int);
    IMPLEMENT_SERIALIZE(SimpleSerializer, size_t);
    IMPLEMENT_SERIALIZE(SimpleSerializer, float);
    IMPLEMENT_SERIALIZE(SimpleSerializer, double);
    
    void SimpleSerializer::SerializeValue(const char* name, const ISerializable& value)
    {
        value.Serialize(*this);
    }

    void SimpleSerializer::SerializeValue(const char* name, const std::vector<int>& array)
    {
        std::cout << "[";
        for(const auto& item: array)
        {
            Serialize(item);
            std::cout << " ";
        }
        std::cout << "]";
    }

    // void SimpleSerializer::SerializeValue(const char* name, const std::vector<const ISerializable&>& array)
    // {

    // }

    void SimpleSerializer::Deserialize(const char* name, ISerializable& value)
    {

    }
}
