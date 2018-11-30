////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnrolledConvolution.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Convolution.h"

#include <math/include/Tensor.h>
#include <math/include/Vector.h>

namespace ell
{
namespace dsp
{
    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DUnrolled(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DUnrolled(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DUnrolled(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride);

    template <typename ValueType>
    void ReceptiveFieldToColumns(math::ConstChannelColumnRowTensorReference<ValueType> input, int filterSize, int stride, math::RowMatrix<ValueType>& shapedInput);
} // namespace dsp
} // namespace ell
