////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WindowTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WindowTest.h"
#include "DSPTestData.h"

// dsp
#include "WindowFunctions.h"

// math
#include "MathConstants.h"

// testing
#include "testing.h"

// stl
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace ell;
using namespace math;
using namespace dsp;

static const double epsilon = 1e-5;

template <typename ValueType>
void TestHammingWindow()
{
    // Hamming window:
    // w(n) = alpha - beta*cos((2*pi*n) / (N-1));
    // with alpha = 0.54, beta = (1-alpha) = 0.46
    const auto size = 100;
    auto window = HammingWindow<ValueType>(size);
    for (size_t index = 0; index < size; ++index)
    {
        ValueType w = static_cast<ValueType>(0.54 - 0.46 * std::cos(2 * Constants<double>::pi * index / (size - 1)));
        testing::ProcessTest("Testing Hamming window generation", testing::IsEqual(window[index], w, static_cast<ValueType>(epsilon)));
    }
}

template <typename ValueType, typename ValueType2>
void VerifyHammingWindow(const std::vector<ValueType>& computed, const std::vector<ValueType2>& reference, std::string symmetry)
{
    using namespace std::string_literals;
    std::vector<ValueType> cast_reference(reference.size());
    std::transform(reference.begin(), reference.end(), cast_reference.begin(), [](ValueType2 x){ return static_cast<ValueType>(x); });
    testing::ProcessTest("Testing "s + symmetry + " Hamming window against generated example", testing::IsEqual(computed, cast_reference, static_cast<ValueType>(epsilon)));
}

template <typename ValueType>
void TestHammingWindow2()
{
    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(128, WindowSymmetry::symmetric), GetReferenceHammingWindow_128_symmetric(), "symmetric");
    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(301, WindowSymmetry::symmetric), GetReferenceHammingWindow_301_symmetric(), "symmetric");
    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(1000, WindowSymmetry::symmetric), GetReferenceHammingWindow_1000_symmetric(), "symmetric");

    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(128, WindowSymmetry::periodic), GetReferenceHammingWindow_128_periodic(), "periodic");
    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(301, WindowSymmetry::periodic), GetReferenceHammingWindow_301_periodic(), "periodic");
    VerifyHammingWindow<ValueType>(HammingWindow<ValueType>(1000, WindowSymmetry::periodic), GetReferenceHammingWindow_1000_periodic(), "periodic");
}

template <typename ValueType>
void TestHannWindow()
{
    // Hann window:
    // w(n) = alpha - beta*cos((2*pi*n) / (N-1));
    // with alpha = beta = 0.5

    const size_t size = 128;
    auto window = HannWindow<ValueType>(size);
    for (size_t index = 0; index < size; ++index)
    {
        ValueType w = static_cast<ValueType>(0.5 - 0.5 * std::cos(2 * Constants<double>::pi * index / (size - 1)));
        testing::ProcessTest("Testing Hann window generation", testing::IsEqual(window[index], w, static_cast<ValueType>(epsilon)));
    }
}

//
// Explicit instantiations
//
template void TestHammingWindow<float>();
template void TestHammingWindow<double>();
template void TestHammingWindow2<float>();
template void TestHammingWindow2<double>();
template void TestHannWindow<float>();
template void TestHannWindow<double>();
