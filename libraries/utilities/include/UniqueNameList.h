////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UniqueNameList.h (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <string>

namespace ell
{
namespace utilities
{
    /// <summary> This class provides uniquification of a list of names by appending integer indexes as needed to ensure each given name is unique </summary>
    class UniqueNameList
    {
    public:
        /// <summary> Adds a new name and make it unique relative to all other added names in this list </summary>
        ///
        /// <returns> The unique name </returns>
        std::string Add(const std::string& name);

    private:
        std::map<std::string, size_t> _table;
    };

} // namespace utilities
} // namespace ell
