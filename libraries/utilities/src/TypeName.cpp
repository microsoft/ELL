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
        for (auto t : subtypes)
        {
            result += t + ",";
        }
        result.back() = '>';
        return result;
    }
}
