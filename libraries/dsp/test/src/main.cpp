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
#include <iostream>
#include <vector>

using namespace ell;
using namespace dsp;

int main()
{
    //
    // Testing
    //

    // 1D Convolution
    TestConv1D<float>(ell::dsp::ConvolutionMethodOption::simple);
    TestConv1D<float>(ConvolutionMethodOption::winograd);
    TestConv1D<float>(ConvolutionMethodOption::winograd);
    TestConv1DVsSimple<float>(32, 3, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv1DVsSimple<float>(33, 3, ell::dsp::ConvolutionMethodOption::winograd);    
    
    // 2D Convolution with matrices
    TestConv2D<float>(ell::dsp::ConvolutionMethodOption::simple);
    TestConv2D<float>(ConvolutionMethodOption::winograd);
    TestConv2D<float>(ConvolutionMethodOption::winograd);

    TestConv2DMatrixVsSimple<float>(120, 80, 3, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DMatrixVsSimple<float>(121, 80, 3, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DMatrixVsSimple<float>(122, 80, 3, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DMatrixVsSimple<float>(121, 81, 3, ell::dsp::ConvolutionMethodOption::winograd);

    // 2D Convolution over tensors

    // Unrolled
    TestConv2DTensorVsSimple<float>(120, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::unrolled);
    TestConv2DTensorVsSimple<float>(121, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::unrolled);
    TestConv2DTensorVsSimple<float>(122, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::unrolled);
    TestConv2DTensorVsSimple<float>(121, 81, 8, 3, 16, ell::dsp::ConvolutionMethodOption::unrolled);
    
    // Winograd
    TestConv2DTensorVsSimple<float>(120, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(121, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(122, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(121, 81, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);

    TestConv2DTensorVsSimple<float>(120, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(121, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(122, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(121, 81, 8, 3, 16, ell::dsp::ConvolutionMethodOption::winograd);
    TestConv2DTensorVsSimple<float>(120, 80, 8, 3, 16, ell::dsp::ConvolutionMethodOption::unrolled);

    TestConv2DTensorVsSimple<float>(60, 40, 64, 3, 128, ell::dsp::ConvolutionMethodOption::unrolled);
    TestConv2DTensorVsSimple<float>(60, 40, 64, 3, 128, ell::dsp::ConvolutionMethodOption::winograd); // Don't test version 1 --- it's too slow

    //
    // Timing
    //

    // 1D Convolution timing
    // args: size, num_iter, method
    TimeConv1D<float>(2000, 2000, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv1D<float>(2000, 2000, ConvolutionMethodOption::winograd);
    std::cout << "\n";

    // 2D Convolution timing
    TimeConv2D<float>(200, 200, 100, ell::dsp::ConvolutionMethodOption::simple);
    // TimeConv2D<float>(200, 200, 100, ConvolutionMethodOption::unrolled); // Matrix version of unrolled convolution currently unimplemented
    TimeConv2D<float>(200, 200, 100, ConvolutionMethodOption::winograd);
    std::cout << "\n";

    // equivalent problem to matrix-based Winograd:
    TimeConv2DTensor<float>(200, 200, 1, 3, 1, 100, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2DTensor<float>(200, 200, 1, 3, 1, 100, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2DTensor<float>(200, 200, 1, 3, 1, 100, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    TimeConv2DTensor<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2DTensor<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2DTensor<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";
    
    TimeConv2DTensor<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2DTensor<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2DTensor<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";
    
    TimeConv2DTensor<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2DTensor<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2DTensor<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";
    
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
