////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StringUtil.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace features
{
    std::string TrimString(const std::string& str);

    std::vector<int> ParseIntRangeString(const std::string& str);
    std::vector<double> ParseValueListString(const std::string& str);
    int ParseInt(const std::string& str);
    double ParseDouble(const std::string& str);

    std::string to_string(const std::vector<double>& list);

    template <typename T>
    std::string to_string(const std::vector<T>& list);

    // Fast routine for printing floats with a small (in magnitude) exponent:
    int sprint_float_fast(char* buf, int n, int p, double val);
}

#include "../tcc/StringUtil.tcc"