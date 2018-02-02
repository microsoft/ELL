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

// utilities
#include "Unused.h"

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

    /// <summary> 
    /// Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal,
    /// returning the magnitudes of the frequency bands. 
    /// </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    ///
    /// <remarks> The output of a real-valued FFT is symmetric, so only the first (N/2)+1 entries of the signal input are necessary </remarks>
    template <typename ValueType>
    void FFT(math::RowVector<ValueType>& signal, bool inverse = false);

    /// <summary> 
    /// Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal,
    /// returning the magnitudes of the frequency bands. 
    /// </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    ///
    /// <remarks> The output of a real-valued FFT is symmetric, so only the first (N/2)+1 entries of the signal input are necessary </remarks>
    template <typename ValueType>
    void FFT(std::vector<ValueType>& signal, bool inverse = false);
}
}

#include "../tcc/FFT.tcc"
