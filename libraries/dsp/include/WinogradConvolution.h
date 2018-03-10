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
    // Main convolution entry points
    //

    enum class WinogradAlgorithmVersion
    {
        v1,
        v2
    };

    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

    /// <summary> Convolve a 1D input with a 1D filter. </summary>
    ///
    /// <param name="input"> The input signal. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A vector with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DWinograd(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter, int tileSize, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

    /// <summary> Convolve a single-channel 2D image with a 2D filter. </summary>
    ///
    /// <param name="input"> The input image. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A matrix with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DWinograd(const math::ConstRowMatrixReference<ValueType>& input, const math::ConstRowMatrixReference<ValueType>& filter, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

    /// <summary> Convolve a single-channel 2D image with a 2D filter. </summary>
    ///
    /// <param name="input"> The input image. </param>
    /// <param name="filter"> The filter to convolve with. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A matrix with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DWinograd(const math::ConstRowMatrixReference<ValueType>& input, const math::ConstRowMatrixReference<ValueType>& filter, int tileSize, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v1);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ChannelColumnRowTensor<ValueType>& input, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

    /// <summary> Spatially convolve a 3D image with a stack of 3D filters. </summary>
    ///
    /// <param name="input"> The input image: a (r x c x d) tensor. </param>
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="version"> An algorithm-specific version number. </param>
    ///
    /// <returns> A tensor with the result of the convolution `input` (*) `filter`
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DWinograd(const math::ChannelColumnRowTensor<ValueType>& input, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, int tileSize, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

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

    /// <summary> Returns a filter tensor transformed into a form usable directly by Winograd convolution. </summary>
    ///
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> GetTransformedFilters(math::ConstChannelColumnRowTensorReference<ValueType> filters, int numFilters, int tileSize, WinogradAlgorithmVersion version = WinogradAlgorithmVersion::v2);

    /// <summary> Transforms a filter tensor into a form usable directly by Winograd convolution, using an existing output tensor. </summary>
    ///
    /// <param name="filters"> The filters to convolve with. A (nf x fr x fc x d) tensor, reshaped as a ((nf*fr) x fc x d) 3D tensor. </param>
    /// <param name="numFilters"> The number of filters in the `filters` argument. </param>
    /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
    /// <param name="transformedFilters"> (Output) The transformed filter. </param>
    template <typename ValueType>
    void TransformFilters(math::ConstChannelColumnRowTensorReference<ValueType> filters, int numFilters, int tileSize, math::ChannelColumnRowTensorReference<ValueType> transformedFilters);
}
}
