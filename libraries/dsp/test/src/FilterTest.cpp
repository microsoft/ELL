////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FilterTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIRFilter.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <vector>

using namespace ell;
using namespace dsp;

template <typename ValueType>
void TestIIRFilter()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    // Simple pre-emph filter
    IIRFilter<ValueType> filter1({ static_cast<ValueType>(1.0) }, { static_cast<ValueType>(-0.95) });
    ValueType y;
    y = filter1.FilterSample(1.0);
    testing::ProcessTest("Testing preemphasis of impulse signal", testing::IsEqual(y, 1.0));
    y = filter1.FilterSample(0.0);
    testing::ProcessTest("Testing preemphasis of impulse signal", testing::IsEqual(y, 0.95, epsilon));
    y = filter1.FilterSample(0.0);
    testing::ProcessTest("Testing preemphasis of impulse signal", testing::IsEqual(y, 0.95 * 0.95, epsilon));
    y = filter1.FilterSample(0.0);
    testing::ProcessTest("Testing preemphasis of impulse signal", testing::IsEqual(y, 0.95 * 0.95 * 0.95, epsilon));
}

template <typename ValueType>
void TestIIRFilterMultiSample()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    // Simple pre-emph filter
    IIRFilter<ValueType> filter1({ static_cast<ValueType>(1.0) }, { static_cast<ValueType>(-0.95) });
    auto y = filter1.FilterSamples({ 1.0, 0.0, 0.0, 0.0 });
    testing::ProcessTest("Testing preemphasis of impulse signal", testing::IsEqual(y, { static_cast<ValueType>(1.0), static_cast<ValueType>(0.95), static_cast<ValueType>(0.95 * 0.95), static_cast<ValueType>(0.95 * 0.95 * 0.95) }, epsilon));
}

template <typename ValueType>
void TestIIRFilterImpulse()
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);

    // Simple pre-emph filter
    std::vector<ValueType> bCoeffs = { static_cast<ValueType>(1.0), static_cast<ValueType>(0.5), static_cast<ValueType>(-0.25), static_cast<ValueType>(0.0125) };
    IIRFilter<ValueType> filter(bCoeffs, { 0 });
    auto y = filter.FilterSamples({ 1.0, 0.0, 0.0, 0.0 });
    testing::ProcessTest("Testing FIR filtering of impulse signal", testing::IsEqual(y, bCoeffs, epsilon));
}

//
// Explicit instantiations
//
template void TestIIRFilter<float>();
template void TestIIRFilter<double>();

template void TestIIRFilterMultiSample<float>();
template void TestIIRFilterMultiSample<double>();

template void TestIIRFilterImpulse<float>();
template void TestIIRFilterImpulse<double>();
