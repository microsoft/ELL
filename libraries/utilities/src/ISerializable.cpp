////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable.h"

// stl
#include <string>
#include <iostream>

namespace utilities
{
    //
    // SimpleSerializer
    //
    void SimpleSerializer::SerializeType(std::string typeName)
    {
        std::cout << std::string(2*_indent, ' ');
        std::cout << "type: " << typeName << std::endl;
    }

    void SimpleSerializer::SerializeField(std::string name, const Variant& variant)
    {
        std::cout << std::string(2*_indent, ' ');
        if(variant.IsPrimitiveType())
        {            
            std::cout << "P:" << name << ": " << to_string(variant) << std::endl;
        }
        else if(variant.IsSerializable())
        {
            std::cout << "S:" << name << ": " << std::endl;
            ++_indent;
            Serialize(variant);
            --_indent;
        }
        else
        {
            // punt
            std::cout << "Cannot serialize field " << name << std::endl;
        }
    }
}
