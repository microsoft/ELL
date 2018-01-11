////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FFT.h (dsp)
//  Authors:  James Devine, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include "MathConstants.h"
#include "Vector.h"

// stl
#include <cassert>
#include <complex>
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> Perform an in-place discrete ("fast") fourier transform (FFT) of a complex-valued input signal. </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    template <typename ValueType>
    void FFT(std::vector<std::complex<ValueType>>& signal, bool inverse = false);

    /// <summary> Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal. </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    template <typename ValueType>
    void FFT(math::RowVector<ValueType>& signal, bool inverse = false);

    /// <summary> Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal. </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    template <typename ValueType>
    void FFT(std::vector<ValueType>& signal, bool inverse = false);
}
}

#include "../tcc/FFT.tcc"
