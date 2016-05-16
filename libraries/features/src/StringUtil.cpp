////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StringUtil.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringUtil.h"

#include <cstring>
#include <string>
#include <cctype>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <vector>

namespace features
{
    std::string TrimString(const std::string &s)
    {
        using std::isspace;
        auto firstWhitespace = std::find_if_not(s.begin(), s.end(), [](int c) { return isspace(c); });
        auto lastWhitespace = std::find_if_not(s.rbegin(), s.rend(), [](int c) { return isspace(c); }).base();
        return (lastWhitespace <= firstWhitespace ? std::string() : std::string(firstWhitespace, lastWhitespace));
    }

    // comma-separated ints or ranges. Ex:   1,2,5;  1-4,6,7,9-10; ...
    std::vector<int> ParseIntRangeString(const std::string& str)
    {
        std::stringstream argStream(str);

        std::vector<int> result;
        std::string field;
        while (std::getline(argStream, field, ','))
        {
            size_t dashPos = field.find_first_of('-');
            if (dashPos != std::string::npos)
            {
                std::string leftPart = field.substr(0, dashPos);
                std::string rightPart = field.substr(dashPos + 1);
                for (int c = stoi(leftPart); c < stoi(rightPart) + 1; c++)
                {
                    result.push_back(c);
                }
            }
            else
            {
                result.push_back(stoi(field));
            }
        }

        return result;
    }

    // TODO: make it accept "[val1, val2, val3]" --- in a json-like format
    std::vector<double> ParseValueListString(const std::string& str)
    {
        std::stringstream argStream(str);
        std::vector<double> result;
        std::string field;
        while (std::getline(argStream, field, ','))
        {
            result.push_back(stod(field));
        }
        return result;
    }

    int ParseInt(const std::string& str)
    {
        return stoi(str);
    }

    double ParseDouble(const std::string& str)
    {
        return std::stod(str);
    }

    std::string to_string(const std::vector<double>& list)
    {
        std::stringstream resultStream;
        resultStream.precision(10);
        bool isFirst = true;
        for (auto val : list)
        {
            if (!isFirst)
            {
                resultStream << ',';
            }

            isFirst = false;
            resultStream << val;
        }
        return resultStream.str();
    }

    // Fast float output:
    int sprint_uint_fast(char* buf, int n, unsigned int val)
    {
        if (val == 0)
        {
            buf[0] = '0';
            buf[1] = '\0';
            return 1;
        }

        const int maxbuf = 20; // way conservative
        char tempbuf[maxbuf + 1];
        tempbuf[maxbuf] = '\0';

        int curr_index = maxbuf;
        while (val != 0 && curr_index > 0)
        {
            --curr_index;
            tempbuf[curr_index] = '0' + (val % 10);
            val /= 10;
        }

        if (val != 0)
        {
            return -1;
        }

        // now copy
        memcpy(buf, tempbuf + curr_index, maxbuf - curr_index + 1); // includes trailing null
        return maxbuf - curr_index;
    }

    int sprint_frac_fast(char* buf, int n, double val)
    {
        assert(val >= 0);

        int curr_index = 0;

        // TODO: should round instead of truncate on last digit
        while (val > 0 && curr_index < n - 1)
        {
            val = val * 10;
            buf[curr_index] = '0' + int(val);
            val = val - int(val);
            curr_index++;
        }
        buf[curr_index] = '\0';
        return curr_index;
    }

    int sprint_float_fast(char* buf, int n, int p, double val)
    {
        if (!std::isfinite(val))
        {
            return snprintf(buf, n, "%f", val);
        }

        int neg_space = 0;
        if (val < 0)
        {
            *buf++ = '-';
            n -= 1;
            val = -val;
            neg_space = 1;
        }

        // print integer part
        int len = sprint_uint_fast(buf, n, (int)val);
        assert(len > 0);
        n -= len;
        buf += len;

        double frac_part = val - int(val);
        if (frac_part > 0)
        {
            // decimal pt
            *buf++ = '.';
            n -= 1;

            // now print fractional part
            int len_frac = sprint_frac_fast(buf, std::min(n, p), frac_part);
            assert(len_frac >= 0);

            return len + len_frac + neg_space + 1; // +1 for decimal point
        }
        else
        {
            *buf = '\0';
            return len + neg_space;
        }
    }
}
