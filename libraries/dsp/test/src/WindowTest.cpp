////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WindowTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WindowFunctions.h"

// math
#include "MathConstants.h"

// testing
#include "testing.h"

// stl
#include <cmath>
#include <iostream>
#include <vector>

using namespace ell;
using namespace math;
using namespace dsp;

static const double epsilon = 1e-5;
static const std::vector<double> g_testHammingWindow = 
{
    #include "HammingWindowCoefficients.inc"
};

template <typename ValueType>
void TestHammingWindow()
{
    // Hamming window:
    // w(n) = alpha - beta*cos((2*pi*n) / (N-1));
    // with alpha = 0.54, beta = (1-alpha) = 0.46
    const auto size = g_testHammingWindow.size();
    std::vector<ValueType> testWindow(g_testHammingWindow.begin(), g_testHammingWindow.end());
    auto window = HammingWindow<ValueType>(size);
    testing::ProcessTest("Testing Hamming window against stored example", testing::IsEqual(window, testWindow, static_cast<ValueType>(epsilon)));

    for (size_t index = 0; index < size; ++index)
    {
        ValueType w = static_cast<ValueType>(0.54 - 0.46 * std::cos(2 * Constants<double>::pi * index / (size - 1)));
        testing::ProcessTest("Testing Hamming window generation", testing::IsEqual(window[index], w, static_cast<ValueType>(epsilon)));
    }
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
template void TestHannWindow<float>();
template void TestHannWindow<double>();
