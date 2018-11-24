////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Convolution.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Convolution.h"

// math
#include "Tensor.h"
#include "Vector.h"

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
    math::RowVector<ValueType> Convolve1DSimple(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride);

    /// <summary> Convolve a set of 2D images with a corresponding set of 2D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (fr x fc x d) tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimpleDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride);

    //
    // Versions that accept the result storage
    //
    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="result"> A vector to store the result in. Must be of length `input.Size() - filter.Size() + 1` or greater. </param>
    template <typename ValueType>
    void Convolve1DSimple(math::ConstRowVectorReference<ValueType> input, const math::RowVector<ValueType>& filter, math::RowVectorReference<ValueType> result);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor.. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    /// <param name="result"> The tensor to write the result into. </param>
    template <typename ValueType>
    void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<ValueType> input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride, math::ChannelColumnRowTensorReference<ValueType> result);
} // namespace dsp
} // namespace ell
