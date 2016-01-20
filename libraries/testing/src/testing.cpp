// testing.cpp

#include "testing.h"

#include "types.h"

// stl
#include <iostream>
using std::cout;
using std::endl;

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

    bool isEqual(const vector<double>& a, const vector<double>& b, double tolerance)
    {
        // allow vectors of different size, provided that they differ by a suffix of zeros
        uint64 size = a.size();
        if (b.size() < size)
        {
            size = b.size();
        }

        for (uint64 i = 0; i < size; ++i)
        {
            if (isEqual(a[i], b[i], tolerance) == false)
            {
                return false;
            }
        }

        // confirm suffix of zeros
        for (uint64 i = size; i < a.size(); ++i)
        {
            if (isEqual(a[i], 0, tolerance) == false)
            {
                return false;
            }
        }

        for (uint64 i = size; i < b.size(); ++i)
        {
            if (isEqual(b[i], 0, tolerance) == false)
            {
                return false;
            }
        }

        return true;
    }

    bool testFailedFlag = false;

    void processTest(const string& testDescription, bool success)
    {
        cout << testDescription << " ... ";

        if (success)
        {
            cout << "Passed\n";
        }
        else
        {
            cout << "Failed\n";
            testFailedFlag = true;
        }
    }

    bool testFailed()
    {
        return testFailedFlag;
    }
}
