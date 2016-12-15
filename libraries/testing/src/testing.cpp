////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     testing.cpp (testing)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{
namespace testing
{
    template <typename ValueType>
    bool IsScalarEqual(const ValueType& a, const ValueType& b)
    {
        return a == b;
    }

    bool IsEqual(bool a, bool b)
    {
        return IsScalarEqual(a, b);
    }

    bool IsEqual(char a, char b)
    {
        return IsScalarEqual(a, b);
    }

    bool IsEqual(int a, int b)
    {
        return IsScalarEqual(a, b);
    }

    bool IsEqual(size_t a, size_t b)
    {
        return IsScalarEqual(a, b);
    }

    bool IsEqual(const std::string& a, const std::string& b)
    {
        return IsScalarEqual(a, b);
    }

    bool IsEqual(float a, float b, float tolerance)
    {
        return (a - b < tolerance && b - a < tolerance);
    }

    bool IsEqual(double a, double b, double tolerance)
    {
        return (a - b < tolerance && b - a < tolerance);
    }

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
    template <typename ValueType>
    bool IsVectorApproxEqual(const std::vector<ValueType>& a, const std::vector<ValueType>& b, ValueType tolerance)
    {
        // allow vectors of different size, provided that they differ by a suffix of zeros
        uint64_t size = a.size();
        if (b.size() < size)
        {
            size = b.size();
        }

        for (uint64_t i = 0; i < size; ++i)
        {
            if (IsEqual(a[i], b[i], tolerance) == false)
            {
                return false;
            }
        }

        // confirm suffix of zeros
        for (uint64_t i = size; i < a.size(); ++i)
        {
            if (IsEqual(a[i], 0, tolerance) == false)
            {
                return false;
            }
        }

        for (uint64_t i = size; i < b.size(); ++i)
        {
            if (IsEqual(b[i], 0, tolerance) == false)
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

    bool IsEqual(const std::vector<float>& a, const std::vector<float>& b, float tolerance)
    {
        return IsVectorApproxEqual(a, b, tolerance);
    }

    bool IsEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance)
    {
        return IsVectorApproxEqual(a, b, tolerance);
    }

    bool testFailedFlag = false;

    void ProcessTest(const std::string& testDescription, bool success)
    {
        std::cout << testDescription << " ... ";

        if (success)
        {
            std::cout << "Passed\n";
        }
        else
        {
            std::cout << "Failed\n";
            testFailedFlag = true;
        }
    }

    bool DidTestFail()
    {
        return testFailedFlag;
    }
}
}
