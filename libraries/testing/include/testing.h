// testing.h

#pragma once

// stl
#include <string>
#include <vector>

namespace testing
{
    /// checks if two doubles are equal, up to a small numerical error
    ///
    bool isEqual(double a, double b, double tolerance = 1.0e-8);

    /// checks if two vectors are equal, up to a small numerical error in each coordinate
    ///
    bool isEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance = 1.0e-8);

    void processTest(const std::string& testDescription, bool success);

    bool testFailed();
}
