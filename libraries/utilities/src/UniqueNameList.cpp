////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UniqueNameList.cpp (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UniqueNameList.h"

namespace ell
{
namespace utilities
{
    std::string UniqueNameList::Add(const std::string& name)
    {
        std::string result = name;
        auto count = _table[name];
        if (count > 0)
        {
            result += std::to_string(count);
        }
        _table[name] += 1;
        return result;
    }
} // namespace utilities
} // namespace ell