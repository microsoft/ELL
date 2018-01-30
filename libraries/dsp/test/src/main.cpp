////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DCTTest.h"
#include "DSPTestData.h"
#include "FFTTest.h"
#include "FilterTest.h"
#include "MelTest.h"
#include "WindowTest.h"

// dsp
#include "FFT.h"
#include "IIRFilter.h"
#include "WindowFunctions.h"

// testing
#include "testing.h"

// stl
#include <complex>
#include <iostream>
#include <vector>

using namespace ell;
using namespace dsp;

int main()
{
    // FFT
    TestFFT<float>(16);
    TestFFT<double>(16);
    VerifyFFT<float>();
    VerifyFFT<double>();

    // Filters
    TestIIRFilter<float>();
    TestIIRFilterMultiSample<float>();
    TestIIRFilterImpulse<float>();

    // Window functions
    TestHammingWindow<float>();
    TestHammingWindow<double>();
    TestHammingWindow2<float>();
    TestHammingWindow2<double>();
    TestHannWindow<float>();
    TestHannWindow<double>();

    // Mel filterbank
    TestMelFilterBank();
    // TestMelFilterBank2(); // Commented out because our implementation rounds filter centers to integer locations, and the reference (librosa) doesn't

    // DCT
    TestDCT();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
