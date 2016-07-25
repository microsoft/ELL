////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectDescription.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ObjectDescription.h"
#include "Variant.h"

// stl
#include <string>
#include <ostream>

namespace utilities
{
    //
    // ObjectDescription
    //
    void ObjectDescription::Print(std::ostream& os, size_t indent)
    {
        const std::string indentStr = std::string(indent, ' ');
        for (const auto& pair : _description)
        {
            os << indentStr << pair.first << ": " << pair.second.GetStoredTypeName() << " = " << to_string(pair.second) << std::endl;
        }
    }
}
