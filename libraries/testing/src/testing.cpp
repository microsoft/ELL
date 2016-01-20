// testing.cpp

#include "testing.h"

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
        if (a.size() != b.size())
        {
            return false;
        }
        for (int i = 0; i < a.size(); ++i)
        {
            if (isEqual(a[i], b[i], tolerance) == false)
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
