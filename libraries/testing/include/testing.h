////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     testing.h (testing)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>
#include <vector>

namespace emll
{
/// <summary> testing namespace </summary>
namespace testing
{
    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    /// <typeparam name="ValueType"> The type of the values. </typeparam>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(bool a, bool b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    /// <typeparam name="ValueType"> The type of the values. </typeparam>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(int a, int b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    /// <typeparam name="ValueType"> The type of the values. </typeparam>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(char a, char b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    /// <typeparam name="ValueType"> The type of the values. </typeparam>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(size_t a, size_t b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    /// <typeparam name="ValueType"> The type of the values. </typeparam>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(std::string a, std::string b);

    /// <summary> Checks if two floats are equal, up to a small numerical error. </summary>
    ///
    /// <param name="a"> The first number. </param>
    /// <param name="b"> The second number. </param>
    /// <param name="tolerance"> The tolerance. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(float a, float b, float tolerance = 1.0e-8);

    /// <summary> Checks if two doubles are equal, up to a small numerical error. </summary>
    ///
    /// <param name="a"> The first number. </param>
    /// <param name="b"> The second number. </param>
    /// <param name="tolerance"> The tolerance. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(double a, double b, double tolerance = 1.0e-8);

    /// <summary>
    /// Checks if two vectors are equal.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<bool>& a, const std::vector<bool>& b);

    /// <summary>
    /// Checks if two vectors are equal.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<int>& a, const std::vector<int>& b);

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
}
