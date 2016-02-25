////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     testing.cpp (testing)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// testing
#include "testing.h"

// stl
#include <iostream>

namespace testing
{
    bool isEqual(double a, double b, double tolerance)
    {
        if (a - b < tolerance && b - a < tolerance)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool isEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance)
    {
        // allow vectors of different size, provided that they differ by a suffix of zeros
        size_t size = a.size();
        if (b.size() < size)
        {
            size = b.size();
        }

        for (size_t i = 0; i < size; ++i)
        {
            if (isEqual(a[i], b[i], tolerance) == false)
            {
                return false;
            }
        }

        // confirm suffix of zeros
        for (size_t i = size; i < a.size(); ++i)
        {
            if (isEqual(a[i], 0, tolerance) == false)
            {
                return false;
            }
        }

        for (size_t i = size; i < b.size(); ++i)
        {
            if (isEqual(b[i], 0, tolerance) == false)
            {
                return false;
            }
        }

        return true;
    }

    bool testFailedFlag = false;

    void processTest(const std::string& testDescription, bool success)
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

    bool testFailed()
    {
        return testFailedFlag;
    }
}
