////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StringUtil.cpp (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringUtil.h"

// stl
#include <algorithm>
#include <cstdio>

namespace ell
{
namespace utilities
{
    std::string ToLowercase(const std::string& s)
    {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](int c) { return static_cast<char>(::tolower(c)); });
        return lower;
    }

    void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty())
            return;
        std::string wsRet;
        wsRet.reserve(str.length());
        size_t startPos = 0;
        size_t pos;
        while ((pos = str.find(from, startPos)) != std::string::npos)
        {
            wsRet += str.substr(startPos, pos - startPos);
            wsRet += to;
            pos += from.length();
            startPos = pos;
        }
        wsRet += str.substr(startPos);
        str.swap(wsRet); // faster than str = wsRet;
    }

    std::vector<std::string> Split(const std::string& s, char separator)
    {
        std::vector<std::string> output;

        std::string::size_type prev_pos = 0, pos = 0;

        while ((pos = s.find(separator, pos)) != std::string::npos)
        {
            std::string substring(s.substr(prev_pos, pos - prev_pos));

            output.push_back(substring);

            prev_pos = ++pos;
        }
        if (prev_pos < pos)
        {
            output.push_back(s.substr(prev_pos, pos - prev_pos));
        }
        return output;

    }

}
}
