////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     testing.h (testing)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>
#include <vector>

namespace ell
{
/// <summary> testing namespace </summary>
namespace testing
{
    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(bool a, bool b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(int a, int b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(char a, char b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(size_t a, size_t b);

    /// <summary> Checks if two values are exactly equal. </summary>
    ///
    /// <param name="a"> The first value. </param>
    /// <param name="b"> The second value. </param>
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
    bool IsEqual(const std::vector<int64_t>& a, const std::vector<int64_t>& b);

    /// <summary>
    /// Checks if two vectors are equal, up to a small numerical error in each coordinate.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    /// <param name="tolerance"> The tolerance. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<float>& a, const std::vector<float>& b, float tolerance = 1.0e-8);
    bool IsEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance = 1.0e-8);

    template <typename ValueType1, typename ValueType2>
    bool IsEqual(const std::vector<std::vector<ValueType1>>& a, const std::vector<std::vector<ValueType2>>& b, double tolerance = 1.0e-8);

    /// <summary>
    /// Checks if two bool vectors are equal.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<bool>& a, const std::vector<bool>& b);

    /// <summary>
    /// Checks if two string vectors are equal.
    /// </summary>
    ///
    /// <param name="a"> The first vector. </param>
    /// <param name="b"> The second vector. </param>
    ///
    /// <returns> true if equal, false if not. </returns>
    bool IsEqual(const std::vector<std::string>& a, const std::vector<std::string>& b);

    /// <summary> Process the test. </summary>
    ///
    /// <param name="testDescription"> Information describing the test. </param>
    /// <param name="success"> true if the test was a success, false if it failed. </param>
    /// <returns> The success value, for convenience </returns>
    bool ProcessTest(const std::string& testDescription, bool success);

    /// <summary> Checks if one of the tests failed. </summary>
    ///
    /// <returns> true if one of the tests failed. </returns>
    bool DidTestFail();
}
}

// Forces a symbol to be defined so that LLVM jit can find it
#if defined(_WIN32)
#if defined(_WIN64)
#define TESTING_FORCE_DEFINE_SYMBOL(x, returnType, ...) __pragma(comment(linker, "/export:" #x))
#else
#define TESTING_FORCE_DEFINE_SYMBOL(x, returnType, ...) __pragma(comment(linker, "/export:_" #x))
#endif
#else
#define TESTING_FORCE_DEFINE_SYMBOL(x, returnType, ...) returnType (*__##x##_fp)(__VA_ARGS__) = &x;
#endif
