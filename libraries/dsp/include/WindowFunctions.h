////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WindowFunctions.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstring> // for size_t
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> Get the values of a Hamming window of a given length. </summary>
    ///
    /// <param name="size"> The length of the window. </param>
    ///
    /// <returns> A vector containing the values of the Hamming window. </returns>
    template <typename ValueType>
    std::vector<ValueType> HammingWindow(size_t size);

    /// <summary> Get the values of a Hann window of a given length. </summary>
    ///
    /// <param name="size"> The length of the window. </param>
    ///
    /// <returns> A vector containing the values of the Hann window. </returns>
    template <typename ValueType>
    std::vector<ValueType> HannWindow(size_t size);

    /// <summary> Get the values of a generalized cosine window window of a given length. </summary>
    ///
    /// <param name="size"> The length of the window. </param>
    /// <param name="coefficients"> The coefficients of the generalized cosine window. </param>
    ///
    /// <returns> A vector containing the values of the generalized cosine window window. </returns>
    template <typename ValueType>
    std::vector<ValueType> GeneralizedCosineWindow(size_t size, const std::vector<double>& coefficients);
}
}

#include "../tcc/WindowFunctions.tcc"
