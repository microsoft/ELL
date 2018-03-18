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
    // 1D
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
    // 2D
    //

    // Input image: r x c x d tensor
    // Filters: nf x fr x fc x d tensor packed into a 3D tensor by collapsing the leading 2 dimensions
    //          (So, a (nf*fr) x fc x d tensor)
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters)
    {
        return Convolve2DSimple(signal, filters, numFilters, 1);
    }

    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ChannelColumnRowTensor<ValueType>& signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, int stride)
    {
        const auto filterRows = filters.NumRows() / numFilters;
        const auto filterColumns = filters.NumColumns();
        const auto outputRows = (signal.NumRows() - filterRows + 1) / stride;
        const auto outputColumns = (signal.NumColumns() - filterColumns + 1) / stride;
        math::ChannelColumnRowTensor<ValueType> result(outputRows, outputColumns, numFilters);
        Convolve2DSimple(signal, filters, numFilters, stride, result);
        return result;
    }

    template <typename ValueType>
    void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<ValueType> signal, const math::ChannelColumnRowTensor<ValueType>& filters, int numFilters, int stride, math::ChannelColumnRowTensorReference<ValueType> result)
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
                    const int inputRowIndex = rowIndex * stride;
                    const int inputColumnIndex = columnIndex * stride;
                    ValueType accum = 0;
                    for (int filterRowIndex = 0; filterRowIndex < filterRows; ++filterRowIndex)
                    {
                        for (int filterColumnIndex = 0; filterColumnIndex < filterColumns; ++filterColumnIndex)
                        {
                            auto signalVector = signal.template GetSlice<math::Dimension::channel>(inputRowIndex + filterRowIndex, inputColumnIndex + filterColumnIndex);
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

    template math::ChannelColumnRowTensor<float> Convolve2DSimple(const math::ChannelColumnRowTensor<float>& signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters);
    template math::ChannelColumnRowTensor<double> Convolve2DSimple(const math::ChannelColumnRowTensor<double>& signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters);

    template math::ChannelColumnRowTensor<float> Convolve2DSimple(const math::ChannelColumnRowTensor<float>& signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters, int stride);
    template math::ChannelColumnRowTensor<double> Convolve2DSimple(const math::ChannelColumnRowTensor<double>& signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters, int stride);

    //
    // Versions with preallocated result
    //
    template void Convolve1DSimple(math::ConstRowVectorReference<float> signal, const math::RowVector<float>& filter, math::RowVectorReference<float> result);
    template void Convolve1DSimple(math::ConstRowVectorReference<double> signal, const math::RowVector<double>& filter, math::RowVectorReference<double> result);

    template void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<float> signal, const math::ChannelColumnRowTensor<float>& filters, int numFilters, int stride, math::ChannelColumnRowTensorReference<float> result);
    template void Convolve2DSimple(math::ConstChannelColumnRowTensorReference<double> signal, const math::ChannelColumnRowTensor<double>& filters, int numFilters, int stride, math::ChannelColumnRowTensorReference<double> result);
}
}
