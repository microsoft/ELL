////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FFTTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FFT.h"

// math
#include "Vector.h"

// testing
#include "testing.h"

// utilities
#include "RandomEngines.h"

// stl
#include <complex>
#include <random>
#include <vector>

using namespace ell;
using namespace dsp;

template <typename ValueType>
void TestFFT(size_t N)
{
    const ValueType epsilon = static_cast<ValueType>(1e-6);
    math::RowVector<ValueType> signal(N);
    std::vector<std::complex<ValueType>> complexSignal(N, 1.0);

    //
    // DC signal
    //
    signal.Fill(1.0);
    FFT(signal);
    testing::ProcessTest("Testing real-valued FFT of DC signal", testing::IsEqual(std::abs(signal[0]), static_cast<ValueType>(N)));
    complexSignal.assign(N, 1.0);
    FFT(complexSignal);
    testing::ProcessTest("Testing FFT of DC signal", testing::IsEqual(std::abs(complexSignal[0]), static_cast<ValueType>(N)));

    //
    // Impulse signal
    //
    signal.Fill(0);
    signal[0] = 1.0;
    FFT(signal);
    for (auto x : signal)
    {
        testing::ProcessTest("Testing real-valued FFT of impulse signal", testing::IsEqual(std::abs(x), static_cast<ValueType>(1)));
    }

    complexSignal.assign(N, 0);
    complexSignal[0] = 1.0;
    FFT(complexSignal);
    for (auto x : complexSignal)
    {
        testing::ProcessTest("Testing FFT of impulse signal", testing::IsEqual(std::abs(x), static_cast<ValueType>(1)));
    }

    //
    // Test agreement between complex and real-valued implementations, random signal
    //
    auto randomEngine = utilities::GetRandomEngine();
    std::uniform_real_distribution<ValueType> uniform(-1, 1);
    for (size_t index = 0; index < N; ++index)
    {
        auto x = uniform(randomEngine);
        signal[index] = x;
        complexSignal[index] = x;
    }

    FFT(signal);
    FFT(complexSignal);
    for (size_t index = 0; index < N; ++index)
    {
        auto x1 = signal[index];
        auto x2 = complexSignal[index];
        testing::ProcessTest("Testing real-valued FFT of random signal", testing::IsEqual(x1, std::abs(x2), epsilon));
    }
}

//
// Explicit instantiation definitions
//
template void TestFFT<float>(size_t);
template void TestFFT<double>(size_t);
