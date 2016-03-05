////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     testing.h (testing)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>
#include <vector>

namespace testing
{
    /// <summary> Checks if two doubles are equal, up to a small numerical error. </summary>
    ///
    /// <param name="a"> The first number. </param>
    /// <param name="b"> The second number. </param>
    /// <param name="tolerance"> The tolerance. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(double a, double b, double tolerance = 1.0e-8);

    /// <summary>
    /// Checks if two vectors are equal, up to a small numerical error in each coordinate.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    /// <param name="tolerance"> The tolerance. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance = 1.0e-8);

    /// <summary> Process the test. </summary>
    ///
    /// <param name="testDescription"> Information describing the test. </param>
    /// <param name="success"> true if the test was a success, false if it failed. </param>
    void ProcessTest(const std::string& testDescription, bool success);

    /// <summary> Checks if one of the tests failed. </summary>
    ///
    /// <returns> true if one of the tests failed. </returns>
    bool DidTestFail();
}
