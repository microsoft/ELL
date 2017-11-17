////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     testing.cpp (testing)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// testing
#include "testing.h"

// utilities
#include "Logger.h"

// stl
#include <iostream>
#include <string>

namespace ell
{
namespace testing
{
    //
    // vectors
    //

    template <typename ValueType>
    bool IsVectorEqual(const std::vector<ValueType>& a, const std::vector<ValueType>& b)
    {
        auto size = a.size();
        if (size != b.size())
        {
            return false;
        }

        for (size_t index = 0; index < size; ++index)
        {
            if (a[index] != b[index])
            {
                return false;
            }
        }
        return true;
    }

    template <typename ValueType1, typename ValueType2, typename ValueType3>
    bool IsVectorApproxEqual(const std::vector<ValueType1>& a, const std::vector<ValueType2>& b, ValueType3 tolerance)
    {
        // allow vectors of different size, provided that they differ by a suffix of zeros
        size_t size = a.size();
        if (b.size() < size)
        {
            size = b.size();
        }

        for (size_t i = 0; i < size; ++i)
        {
            if (IsEqual(a[i], b[i], tolerance) == false)
            {
                return false;
            }
        }

        // confirm suffix of zeros
        for (size_t i = size; i < a.size(); ++i)
        {
            if (IsEqual(a[i], static_cast<ValueType1>(0), tolerance) == false)
            {
                return false;
            }
        }

        for (size_t i = size; i < b.size(); ++i)
        {
            if (IsEqual(b[i], static_cast<ValueType2>(0), tolerance) == false)
            {
                return false;
            }
        }

        return true;
    }

    bool IsEqual(const std::vector<bool>& a, const std::vector<bool>& b)
    {
        return IsVectorEqual(a, b);
    }

    bool IsEqual(const std::vector<int>& a, const std::vector<int>& b)
    {
        return IsVectorEqual(a, b);
    }

    bool IsEqual(const std::vector<int64_t>& a, const std::vector<int64_t>& b)
    {
        return IsVectorEqual(a, b);
    }

    bool IsEqual(const std::vector<std::string>& a, const std::vector<std::string>& b)
    {
        return IsVectorEqual(a, b);
    }

    bool IsEqual(const std::vector<float>& a, const std::vector<float>& b, float tolerance)
    {
        return IsVectorApproxEqual(a, b, tolerance);
    }

    bool IsEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance)
    {
        return IsVectorApproxEqual(a, b, tolerance);
    }

    bool IsEqual(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b, float tolerance)
    {
        for (size_t index = 0; index < a.size(); ++index)
        {
            if (!IsVectorApproxEqual(a[index], b[index], tolerance))
            {
                return false;
            }
        }
        return true;
    }

    template <typename ValueType1, typename ValueType2>
    bool IsEqual(const std::vector<std::vector<ValueType1>>& a, const std::vector<std::vector<ValueType2>>& b, double tolerance)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (size_t index = 0; index < a.size(); ++index)
        {
            if (!IsVectorApproxEqual(a[index], b[index], tolerance))
            {
                return false;
            }
        }
        return true;
    }

    bool testFailedFlag = false;

    bool ProcessTest(const std::string& testDescription, bool success)
    {
        if (!success)
        {
            std::cout << testDescription << " ... Failed\n";
            testFailedFlag = true;
        }

        return success;
    }

    bool DidTestFail()
    {
        return testFailedFlag;
    }

    EnableLoggingHelper::EnableLoggingHelper()
    {
        logging::ShouldLog() = true;
    }

    EnableLoggingHelper::~EnableLoggingHelper()
    {
        logging::ShouldLog() = false;
    }

    template bool IsEqual(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b, double tolerance);
    template bool IsEqual(const std::vector<std::vector<float>>& a, const std::vector<std::vector<double>>& b, double tolerance);
    template bool IsEqual(const std::vector<std::vector<double>>& a, const std::vector<std::vector<float>>& b, double tolerance);
    template bool IsEqual(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b, double tolerance);
}
}
