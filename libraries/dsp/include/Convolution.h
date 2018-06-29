////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Convolution.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"
#include "Vector.h"

namespace ell
{
namespace dsp
{
    // Notational conventions:
    //
    // (*): the convolution operator
    //
    // r: number of rows in the input image
    // c: number of columns in the input image
    // d: the depth (# channels) of input image (and filters)
    // fr: number of rows in the filters
    // fc: number of columns in the filter (typically the same as fr)
    // nf: number of filters
    // NOTE: these functions all compute "valid" convolutions. So the output size = input size - filter size + 1

    /// <summary> The method to use for performing convolutions. </summary>
    /// Note: the values for these enums should be kept in sync with the values of `ConvolutionalLayer::ConvolutionMethod`
    enum class ConvolutionMethodOption : int
    {
        /// <summary> Allow the function to choose the algorithm to use. </summary>
        automatic = 0,
        /// <summary> A different method of doing convolution which avoids reshaping the input, and uses gemm on smaller matrices with diagonal sums to create output. </summary>
        diagonal,
        /// <summary> A simple, straightforward nested-loop implementation. </summary>
        simple,
        /// <summary> An algorithm that reduces the number of arithmetic operations. </summary>
        winograd,
        /// <summary> Normal method of doing convolution via reshaping input into columns and performing a gemm operation. </summary>
        unrolled,
    };

    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input. </param>
    /// <param name="filter"> The filter to convolve the input with. </param>
    /// <param name="method"> The convolution algorithm to use. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1D(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter, ConvolutionMethodOption method = ConvolutionMethodOption::automatic);

    /// <summary> Spatially (in 2D) convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="method"> The convolution algorithm to use. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2D(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, ConvolutionMethodOption method = ConvolutionMethodOption::automatic);

    /// <summary> Spatially (in 2D) convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    /// <param name="method"> The convolution algorithm to use. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2D(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride, ConvolutionMethodOption method = ConvolutionMethodOption::automatic);

    /// <summary> Convolve a set of 2D images with a corresponding set of 2D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (fr x fc x d) tensor. </param>
    /// <param name="method"> The convolution algorithm to use. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, ConvolutionMethodOption method = ConvolutionMethodOption::automatic);

    /// <summary> Convolve a set of 2D images with a corresponding set of 2D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (fr x fc x d) tensor. </param>
    /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
    /// <param name="method"> The convolution algorithm to use. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride, ConvolutionMethodOption method = ConvolutionMethodOption::automatic);

    //
    // Explicit instantiation declarations:
    //
    extern template math::RowVector<float> Convolve1D(const math::RowVector<float>& input, const math::RowVector<float>& filter, ConvolutionMethodOption method);
    extern template math::RowVector<double> Convolve1D(const math::RowVector<double>& input, const math::RowVector<double>& filter, ConvolutionMethodOption method);

    extern template math::ChannelColumnRowTensor<float> Convolve2D(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, ConvolutionMethodOption method);
    extern template math::ChannelColumnRowTensor<double> Convolve2D(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, ConvolutionMethodOption method);

    extern template math::ChannelColumnRowTensor<float> Convolve2D(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, int stride, ConvolutionMethodOption method);
    extern template math::ChannelColumnRowTensor<double> Convolve2D(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, int stride, ConvolutionMethodOption method);

    extern template math::ChannelColumnRowTensor<float> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, ConvolutionMethodOption method);
    extern template math::ChannelColumnRowTensor<double> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, ConvolutionMethodOption method);

    extern template math::ChannelColumnRowTensor<float> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters, int stride, ConvolutionMethodOption method);
    extern template math::ChannelColumnRowTensor<double> Convolve2DDepthwiseSeparable(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters, int stride, ConvolutionMethodOption method);
}
}
