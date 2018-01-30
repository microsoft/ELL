////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MelTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MelTest.h"
#include "CepstrumTestData.h"
#include "DSPTestData.h"

// dsp
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

void VerifyMelFilterBank(double sampleRate, size_t windowSize, size_t numFilters, const std::vector<std::vector<double>>& reference)
{
    using namespace std::string_literals;
    const double epsilon = 1e-6;

    auto m = MelFilterBank(windowSize, sampleRate, numFilters);
    for (size_t filterIndex = 0; filterIndex < numFilters; ++filterIndex)
    {
        auto f = m.GetFilter(filterIndex).ToArray();
        testing::ProcessTest("Testing Mel filter "s + std::to_string(filterIndex), testing::IsEqual(f, reference[filterIndex], epsilon));
    }
}

void TestMelFilterBank()
{
    const size_t numFilters = 13;
    const size_t windowSize = 512;
    const double sampleRate = 16000;

    VerifyMelFilterBank(sampleRate, windowSize, numFilters, GetReferenceCepstrumCoefficients());
}

void TestMelFilterBank2()
{
    VerifyMelFilterBank(16000, 512, 128, GetMelReference_16000_512_128());
    VerifyMelFilterBank(16000, 512, 40, GetMelReference_16000_512_40());
    VerifyMelFilterBank(16000, 512, 13, GetMelReference_16000_512_13());

    VerifyMelFilterBank(8000, 512, 128, GetMelReference_8000_512_128());
    VerifyMelFilterBank(8000, 512, 40, GetMelReference_8000_512_40());
    VerifyMelFilterBank(8000, 512, 13, GetMelReference_8000_512_13());
}
