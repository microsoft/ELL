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
    // SimpleJsonSerializer2
    //
    void SimpleJsonSerializer2::BeginSerializeType(const ObjectDescription& desc)
    {
        auto typeName = desc.GetTypeName();
        std::cout << "{" << std::endl;
        std::cout << std::string(2*_indent, ' ');
        std::cout << "'type': " << typeName;
        if(desc.GetNumFields() > 0) std::cout << ",";
        std::cout << std::endl;
    }

    void SimpleJsonSerializer2::EndSerializeType(const ObjectDescription& desc)
    {
        std::cout << std::string(2*_indent, ' ');
        std::cout << "}";
    }

    void SimpleJsonSerializer2::SerializeFundamentalType(const Variant& variant)
    {
        std::cout << to_string(variant);
    }

    void SimpleJsonSerializer2::SerializeField(std::string name, const Variant& variant)
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
