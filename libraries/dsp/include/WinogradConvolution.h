////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WinogradConvolution.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Convolution.h"

// math
#include "Matrix.h"
#include "Tensor.h"
#include "Vector.h"

// stl
#include <vector>

namespace ell
{
namespace dsp
{
    //
    // <summary> Used to specify the data order in the transformed filters used for Winograd convolution. </summary>
    //   `tilesFirst` is generally more efficient, but `filtersFirst` may be better when the number of filters and channels is small.
    //
    enum class WinogradFilterOrder
    {
        filtersFirst,
        tilesFirst
    };

    //
    // Main convolution entry points
    //

    //
    // 1D convolution
    //

    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter);

    /// <summary> Convolve a 1D input with a 1D filter with a user-specified tile size. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter, int tileSize);

    //
    // 2D convolution
    //

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="order"> The ordering to use for the transformed filters. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, WinogradFilterOrder order = WinogradFilterOrder::tilesFirst);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="order"> The ordering to use for the transformed filters. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int tileSize, WinogradFilterOrder order = WinogradFilterOrder::tilesFirst);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters, using pre-transformed filter weights. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="transformedFilters"> The transformed filters to convolve with. This is obtained by calling output of calling `GetTransformedFilters` or `TransformFilters()`
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="order"> The ordering to use for the transformed filters. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinogradPretransformed(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& transformedFilters, int numFilters, int tileSize, int filterSize, WinogradFilterOrder order = WinogradFilterOrder::tilesFirst);

    //
    // Filter pre-transformation functions
    //

    /// <summary> Returns a filter tensor transformed into a form usable directly by Winograd convolution. </summary>
    ///
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> GetTransformedFilters(const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int tileSize, WinogradFilterOrder order = WinogradFilterOrder::tilesFirst);

    /// <summary> Transforms a filter tensor into a form usable directly by Winograd convolution, using an existing output tensor. </summary>
    ///
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="transformedFilters"> (Output) The transformed filter. </param>
    template <typename ValueType>
    void TransformFilters(const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int tileSize, WinogradFilterOrder order, math::ChannelColumnRowTensorReference<ValueType>& transformedFilters);

    //
    // Winograd convolution implementation functions
    //

    /// <summary> Gets the data-transforming matrix for Winograd convolution (commonly notated as B'). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftDataTransformMatrix(int tileSize, int filterSize);

    /// <summary> Gets the righthand-side data-transforming matrix for 2D Winograd convolution (commonly notated as B). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightDataTransformMatrix(int tileSize, int filterSize);

    /// <summary> Gets the filter-transforming matrix for Winograd convolution (commonly notated as G). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftFilterTransformMatrix(int tileSize, int filterSize);

    /// <summary> Gets the righthand-side filter-transforming matrix for 2D Winograd convolution (commonly notated as G'). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightFilterTransformMatrix(int tileSize, int filterSize);

    /// <summary> Gets the result-transforming matrix for Winograd convolution (commonly notated as A'). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetLeftResultTransformMatrix(int tileSize, int filterSize);

    /// <summary> Gets the righthand-side result-transforming matrix for 2D Winograd convolution (commonly notated as A). </summary>
    ///
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="filterSize"> The size of the filter. </param>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetRightResultTransformMatrix(int tileSize, int filterSize);
}
}
