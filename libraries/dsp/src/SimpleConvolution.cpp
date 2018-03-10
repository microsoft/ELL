////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SimpleConvolution.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleConvolution.h"

// math
#include "VectorOperations.h"

// utilities
#include "Unused.h"

namespace ell
{
namespace dsp
{
    //
    // Simple convolution with nested loops
    //

    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DSimple(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter)
    {
        auto filterSize = filter.Size();
        auto outputSize = signal.Size() - filterSize + 1;
        math::RowVector<ValueType> result(outputSize);
        Convolve1DSimple(signal, filter, result);
        return result;
    }

    template <typename ValueType>
    void Convolve1DSimple(math::ConstRowVectorReference<ValueType> signal, const math::RowVector<ValueType>& filter, math::RowVectorReference<ValueType> result)
    {
        auto filterSize = static_cast<int>(filter.Size());
        auto outputSize = static_cast<int>(result.Size());

        for (int index = 0; index < outputSize; ++index)
        {
            ValueType accum = 0;
            for (int filterIndex = 0; filterIndex < filterSize; ++filterIndex)
            {
                accum += filter[filterIndex] * signal[index + filterIndex];
            }
            result[index] = accum;
        }
    }

    //
    // Simple convolution with nested loops
    //
    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DSimple(const math::ConstRowMatrixReference<ValueType>& signal, const math::ConstRowMatrixReference<ValueType>& filter)
    {
        auto filterRows = filter.NumRows();
        auto filterColumns = filter.NumColumns();
        auto outputRows = signal.NumRows() - filterRows + 1;
        auto outputColumns = signal.NumColumns() - filterColumns + 1;
        math::RowMatrix<ValueType> result(outputRows, outputColumns);
        Convolve2DSimple(signal, filter, result);
        return result;
    }

    template <typename ValueType>
    void Convolve2DSimple(math::ConstRowMatrixReference<ValueType> signal, const math::ConstRowMatrixReference<ValueType>& filter, math::RowMatrixReference<ValueType> result)
    {
        auto filterRows = static_cast<int>(filter.NumRows());
        auto filterColumns = static_cast<int>(filter.NumColumns());
        auto outputRows = static_cast<int>(result.NumRows());
        auto outputColumns = static_cast<int>(result.NumColumns());

        for (int rowIndex = 0; rowIndex < outputRows; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < outputColumns; ++columnIndex)
            {
                ValueType accum = 0;
                for (int filterRowIndex = 0; filterRowIndex < filterRows; ++filterRowIndex)
                {
                    for (int filterColumnIndex = 0; filterColumnIndex < filterColumns; ++filterColumnIndex)
                    {
                        accum += filter(filterRowIndex, filterColumnIndex) * signal(rowIndex + filterRowIndex, columnIndex + filterColumnIndex);
                    }
                }
                result(rowIndex, columnIndex) = accum;
            }
        }
    }

    // Input image: r x c x d tensor
    // Filters: nf x fr x fc x d tensor packed into a 3D tensor by collapsing the leading 2 dimensions
    //          (So, a (nf*fr) x fc x d tensor)
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters)
    {
        const auto filterRows = filters.NumRows() / numFilters;
        const auto filterColumns = filters.NumColumns();
        const auto outputRows = signal.NumRows() - filterRows + 1;
        const auto outputColumns = signal.NumColumns() - filterColumns + 1;
        math::ChannelColumnRowTensor<ValueType> result(outputRows, outputColumns, numFilters);
        Convolve2DSimple(signal, filters, numFilters, result);
        return result;
    }

    template <typename ValueType>
    void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<ValueType> signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, math::ChannelColumnRowTensorReference<ValueType> result)
    {
        const auto filterRows = static_cast<int>(filters.NumRows()) / numFilters;
        const auto filterColumns = static_cast<int>(filters.NumColumns());
        const auto outputRows = static_cast<int>(result.NumRows());
        const auto outputColumns = static_cast<int>(result.NumColumns());

        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            const auto filterOffset = filterIndex * filterRows;
            for (int rowIndex = 0; rowIndex < outputRows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < outputColumns; ++columnIndex)
                {
                    ValueType accum = 0;
                    for (int filterRowIndex = 0; filterRowIndex < filterRows; ++filterRowIndex)
                    {
                        for (int filterColumnIndex = 0; filterColumnIndex < filterColumns; ++filterColumnIndex)
                        {
                            auto signalVector = signal.template GetSlice<math::Dimension::channel>(rowIndex + filterRowIndex, columnIndex + filterColumnIndex);
                            auto filterVector = filters.template GetSlice<math::Dimension::channel>(filterOffset + filterRowIndex, filterColumnIndex);
                            accum += math::Dot(signalVector, filterVector);
                        }
                    }
                    result(rowIndex, columnIndex, filterIndex) = accum;
                }
            }
        }
    }

    //
    // Explicit instantiations
    //

    //
    // Versions that return result
    //
    template math::RowVector<float> Convolve1DSimple(const math::RowVector<float>& signal, const math::RowVector<float>& filter);
    template math::RowVector<double> Convolve1DSimple(const math::RowVector<double>& signal, const math::RowVector<double>& filter);

    template math::RowMatrix<float> Convolve2DSimple(const math::ConstRowMatrixReference<float>& signal, const math::ConstRowMatrixReference<float>& filter);
    template math::RowMatrix<double> Convolve2DSimple(const math::ConstRowMatrixReference<double>& signal, const math::ConstRowMatrixReference<double>& filter);

    template math::ChannelColumnRowTensor<float> Convolve2DSimple(const math::ChannelColumnRowTensor<float>& signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters);
    template math::ChannelColumnRowTensor<double> Convolve2DSimple(const math::ChannelColumnRowTensor<double>& signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters);

    //
    // Versions with preallocated result
    //
    template void Convolve1DSimple(math::ConstRowVectorReference<float> signal, const math::RowVector<float>& filter, math::RowVectorReference<float> result);
    template void Convolve1DSimple(math::ConstRowVectorReference<double> signal, const math::RowVector<double>& filter, math::RowVectorReference<double> result);

    template void Convolve2DSimple(math::ConstRowMatrixReference<float> signal, const math::ConstRowMatrixReference<float>& filter, math::RowMatrixReference<float> result);
    template void Convolve2DSimple(math::ConstRowMatrixReference<double> signal, const math::ConstRowMatrixReference<double>& filter, math::RowMatrixReference<double> result);

    template void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<float> signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters, math::ChannelColumnRowTensorReference<float> result);
    template void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<double> signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters, math::ChannelColumnRowTensorReference<double> result);
}
}
