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
}
