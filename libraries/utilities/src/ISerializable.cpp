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
    // ObjectDescription
    //
    void ObjectDescription::AddField(std::string name, const Variant& value)
    {
        _description.insert(std::make_pair(name, value)); 
    }

    void ObjectDescription::Print(std::ostream& os, size_t indent)
    {
        const std::string indentStr = std::string(indent, ' ');
        for (const auto& pair : _description)
        {
            os << indentStr << pair.first << ": " << pair.second.GetStoredTypeName() << " = " << to_string(pair.second) << std::endl;
        }
    }

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
            std::cout << name << ": " << to_string(variant) << std::endl;
        }
        else if(variant.IsSerializable())
        {
            std::cout << name << ": " << std::endl;
            ++_indent;
            if(variant.IsPointer())
            {
                auto ptr = variant.GetValue<const ISerializable*>();                
                Serialize(*ptr);
            }
            else
            {
                // auto ptr = variant.GetSerializableInterface();
                // Serialize(*ptr);
            }
            --_indent;
        }
        else
        {
            // punt
            std::cout << "Cannot serialize field " << name << std::endl;
        }
    }
}
