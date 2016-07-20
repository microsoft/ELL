////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISerializable.h"

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
            os << indentStr << pair.first << ": " << to_string(pair.second) << std::endl;
        }
    }
}
