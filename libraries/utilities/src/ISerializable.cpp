////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable.h"
#include "Variant.h"
// stl
#include <string>
#include <iostream>

namespace utilities
{
    //
    // SimpleSerializer
    //
    void SimpleSerializer::BeginSerializeType(const ObjectDescription& desc)
    {
        auto typeName = desc.GetTypeName();
        std::cout << "{" << std::endl;
        std::cout << std::string(2*_indent, ' ');
        std::cout << "'type': " << typeName;
        if(desc.GetNumFields() > 0) std::cout << ",";
        std::cout << std::endl;
    }

    void SimpleSerializer::EndSerializeType(const ObjectDescription& desc)
    {
        std::cout << std::string(2*_indent, ' ');
        std::cout << "}";
    }

    void SimpleSerializer::SerializeFundamentalType(const Variant& variant)
    {
        std::cout << to_string(variant);
    }

    void SimpleSerializer::SerializeField(std::string name, const Variant& variant)
    {
        ++_indent;
        std::cout << std::string(2*_indent, ' ');
        if(variant.IsPrimitiveType())
        {
            std::cout << "'" << name << "'" << ": " << to_string(variant);
        }
        else if(variant.IsSerializable())
        {
            std::cout << "'" << name << "'" << ": ";
            ++_indent;
            Serialize(variant);
            --_indent;
        }
        else
        {
            // punt --- should probably throw an exception
            std::cout << "Cannot serialize field " << name << std::endl;
        }
        --_indent;

    }
}
