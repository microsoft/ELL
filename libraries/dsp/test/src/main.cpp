////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionTest.h"
#include "DCTTest.h"
#include "DSPTestData.h"
#include "FFTTest.h"
#include "FilterTest.h"
#include "MelTest.h"
#include "WindowTest.h"

// dsp
#include "Convolution.h"
#include "FFT.h"
#include "IIRFilter.h"
#include "WindowFunctions.h"

// testing
#include "testing.h"

// stl
#include <complex>
#include <vector>

using namespace ell;
using namespace dsp;

int main()
{
    //
    // Testing
    //

    // 1D Convolution
    TestConv1D<float>(ConvolutionMethodOption::simple);
    TestConv1D<float>(ConvolutionMethodOption::winograd);
    TestConv1D<float>(ConvolutionMethodOption::winograd);
    TestConv1DVsSimple<float>(32, 3, ConvolutionMethodOption::winograd);
    TestConv1DVsSimple<float>(33, 3, ConvolutionMethodOption::winograd);    
    
    // 2D Convolution

    // Unrolled
    // stride == 1
    TestConv2DVsSimple<float>(4, 4, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(6, 6, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(120, 80, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(121, 80, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(122, 80, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(121, 81, 8, 3, 16, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(60, 40, 64, 3, 128, 1, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(129, 129, 128, 3, 128, 1, ConvolutionMethodOption::unrolled);

    // stride == 2
    TestConv2DVsSimple<float>(4, 4, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(6, 6, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(120, 80, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(121, 80, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(122, 80, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(121, 81, 8, 3, 16, 2, ConvolutionMethodOption::unrolled);
    TestConv2DVsSimple<float>(60, 40, 64, 3, 128, 2, ConvolutionMethodOption::unrolled);
    
    // Winograd
    TestConv2DVsSimple<float>(4, 4, 1, 3, 1, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(4, 4, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(6, 6, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(120, 80, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(121, 80, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(122, 80, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(121, 81, 8, 3, 16, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(60, 40, 64, 3, 128, 1, ConvolutionMethodOption::winograd);
    TestConv2DVsSimple<float>(129, 129, 128, 3, 128, 1, ConvolutionMethodOption::winograd);
    
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

    return testing::DidTestFail() ? 1 : 0;
}
