////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeName.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace utilities
{
    std::string GetCompositeTypeName(std::string baseType, const std::vector<std::string>& subtypes)
    {
        if (subtypes.size() == 0)
        {
            return baseType;
        }
        std::string result = baseType + "<";
        for (size_t index = 0; index < subtypes.size(); ++index)
        {
            if (index != 0)
            {
                result += ",";
            }
            result += subtypes[index];
        }
        result += ">";
        return result;
    }
}
