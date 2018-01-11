////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MelTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FilterBank.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <vector>

using namespace ell;
using namespace dsp;

template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& arr)
{
    os << "[";
    for (auto x : arr)
    {
        os << x << " ";
    }
    os << "]";
    return os;
}

void TestMelFilterBank()
{
    using ValueType = double;
    const ValueType epsilon = 1e-6;
    const size_t numFilters = 13;
    const size_t windowSize = 512;
    const double sampleRate = 16000;

    const std::vector<std::vector<ValueType>> cepstrum_coefficients =
    {
        #include "CepstrumCoefficients.inc"
    };

    auto m = MelFilterBank(windowSize, sampleRate, numFilters);
    for (size_t filterIndex = 0; filterIndex < numFilters; ++filterIndex)
    {
        auto f = m.GetFilter(filterIndex).ToArray();
        testing::ProcessTest("Testing Mel filter", testing::IsEqual(f, cepstrum_coefficients[filterIndex], epsilon));
    }
}
