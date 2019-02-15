////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StringUtil.cpp (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringUtil.h"
#include "Exception.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <sstream>

namespace ell
{
namespace utilities
{
    bool Contains(const std::string& s, const std::string& substring)
    {
        return s.find(substring) != std::string::npos;
    }

    std::string ToLowercase(const std::string& s)
    {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](int c) { return static_cast<char>(::tolower(c)); });
        return lower;
    }

    std::string ToUppercase(const std::string& s)
    {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](int c) { return static_cast<char>(::toupper(c)); });
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

    std::string Join(const std::vector<std::string>& strings, const std::string& separator)
    {
        std::stringstream stream;
        std::string sep = "";
        for (const auto& s : strings)
        {
            stream << sep << s;
            sep = separator;
        }
        return stream.str();
    }

    std::string MakeValidIdentifier(const std::string& s)
    {
        std::string result;
        for (char c : s)
        {
            if (std::isalnum(c) || c == '_')
            {
                result.push_back(c);
            }
        }
        return result;
    }

    template <>
    bool FromString(const std::string& s)
    {
        auto str = ToLowercase(s);
        if (str == "true")
        {
            return true;
        }
        else if (str == "false")
        {
            return false;
        }
        throw InputException(InputExceptionErrors::invalidArgument, "Error parsing \"" + s + "\" as a boolean");
    }

    template <>
    int FromString(const std::string& s)
    {
        return std::stoi(s);
    }

    template <>
    long FromString(const std::string& s)
    {
        return std::stol(s);
    }

    template <>
    unsigned long FromString(const std::string& s)
    {
        return std::stoul(s);
    }

    template <>
    float FromString(const std::string& s)
    {
        return std::stof(s);
    }

    template <>
    double FromString(const std::string& s)
    {
        return std::stod(s);
    }

    template <>
    std::string FromString(const std::string& s)
    {
        return s;
    }
} // namespace utilities
} // namespace ell
