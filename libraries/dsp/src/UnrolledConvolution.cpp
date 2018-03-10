////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnrolledConvolution.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnrolledConvolution.h"

// math
#include "MatrixOperations.h"

// utilities
#include "Unused.h"

namespace ell
{
namespace dsp
{
    namespace
    {
        template <typename ValueType>
        void ReceptiveFieldToColumns(math::ConstChannelColumnRowTensorReference<ValueType> input, int filterSize, int stride, math::RowMatrix<ValueType>& shapedInput)
        {
            const auto numChannels = static_cast<int>(input.NumChannels());
            const auto fieldVolumeSize = filterSize * filterSize * numChannels;
            const auto numOutputRows = (static_cast<int>(input.NumRows()) - filterSize + 1) / stride;
            const auto numOutputColumns = (static_cast<int>(input.NumColumns()) - filterSize + 1) / stride;

            for (int f = 0; f < fieldVolumeSize; f++)
            {
                const auto fieldDepth = f % numChannels;
                const auto fieldColumn = (f / numChannels) % filterSize;
                const auto fieldRow = (f / numChannels) / filterSize;

                int rowOffset = 0;
                for (int h = 0; h < numOutputRows; h++)
                {
                    int colOffset = 0;
                    for (int w = 0; w < numOutputColumns; w++)
                    {
                        auto input_row = rowOffset + fieldRow;
                        auto input_col = colOffset + fieldColumn;

                        shapedInput(f, h * numOutputColumns + w) = input(input_row, input_col, fieldDepth);
                        colOffset += stride;
                    }
                    rowOffset += stride;
                }
            }
        }

        template <typename ValueType>
        math::RowMatrix<ValueType> ComputeWeightsMatrix(const math::ConstChannelColumnRowTensorReference<ValueType>& weightsTensor)
        {
            const auto numChannels = weightsTensor.NumChannels();
            const auto filterSize = static_cast<int>(weightsTensor.NumColumns());
            const auto numFilters = weightsTensor.NumRows() / filterSize;
            math::RowMatrix<ValueType> weightsMatrix{ numFilters, filterSize * filterSize * numChannels };

            // Reshape the weights
            auto flattened = weightsTensor.ReferenceAsMatrix();
            auto numFlattenedRows = static_cast<int>(flattened.NumRows());
            for (int startRow = 0; startRow < numFlattenedRows / filterSize; startRow++)
            {
                for (int row = 0; row < filterSize; row++)
                {
                    auto weightsVector = flattened.GetMajorVector(startRow * filterSize + row);
                    auto weightsVectorSize = static_cast<int>(weightsVector.Size());
                    for (int i = 0; i < weightsVectorSize; i++)
                    {
                        const int columnOffset = row * weightsVectorSize;
                        weightsMatrix(startRow, columnOffset + i) = weightsVector[i];
                    }
                }
            }
            return weightsMatrix;
        }
    }

    //
    // Unrolled convolution
    //
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DUnrolled(const math::RowVector<ValueType>& input, const math::RowVector<ValueType>& filter)
    {
        UNUSED(input, filter);
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    math::RowMatrix<ValueType> Convolve2DUnrolled(const math::ConstRowMatrixReference<ValueType>& input, const math::ConstRowMatrixReference<ValueType>& filter)
    {
        UNUSED(input, filter);
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DUnrolled(const math::ConstChannelColumnRowTensorReference<ValueType>& input, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters)
    {
        const auto filterSize = static_cast<int>(filters.NumColumns());
        const auto stride = 1;
        const auto numChannels = static_cast<int>(input.NumChannels());
        const auto numOutputRows = (static_cast<int>(input.NumRows()) - filterSize + 1) / stride;
        const auto numOutputColumns = (static_cast<int>(input.NumColumns()) - filterSize + 1) / stride;

        math::RowMatrix<ValueType> shapedInput{ static_cast<size_t>(filterSize * filterSize * numChannels), static_cast<size_t>(numOutputRows * numOutputColumns) };
        math::RowMatrix<ValueType> weightsMatrix = ComputeWeightsMatrix(filters);
        math::RowMatrix<ValueType> outputMatrix{ static_cast<size_t>(numFilters), static_cast<size_t>(numOutputRows * numOutputColumns) };
        math::ChannelColumnRowTensor<ValueType> output{ static_cast<size_t>(numOutputRows), static_cast<size_t>(numOutputColumns), static_cast<size_t>(numFilters) };

        // Re-shape input.
        ReceptiveFieldToColumns(input, filterSize, stride, shapedInput);
        math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), weightsMatrix, shapedInput, static_cast<ValueType>(0.0), outputMatrix);

        // Re-shape the output into the output tensor
        for (int i = 0; i < numOutputRows; i++)
        {
            for (int j = 0; j < numOutputColumns; j++)
            {
                for (int k = 0; k < numFilters; k++)
                {
                    int row = k;
                    int column = (i * numOutputColumns) + j;
                    output(i, j, k) = outputMatrix(row, column);
                }
            }
        }

        return output;
    }

    // Basic entry points
    template math::RowVector<float> Convolve1DUnrolled(const math::RowVector<float>& input, const math::RowVector<float>& filter);
    template math::RowVector<double> Convolve1DUnrolled(const math::RowVector<double>& input, const math::RowVector<double>& filter);

    template math::RowMatrix<float> Convolve2DUnrolled(const math::ConstRowMatrixReference<float>& input, const math::ConstRowMatrixReference<float>& filter);
    template math::RowMatrix<double> Convolve2DUnrolled(const math::ConstRowMatrixReference<double>& input, const math::ConstRowMatrixReference<double>& filter);

    template math::ChannelColumnRowTensor<float> Convolve2DUnrolled(const math::ConstChannelColumnRowTensorReference<float>& input, const math::ConstChannelColumnRowTensorReference<float>& filters, int numFilters);
    template math::ChannelColumnRowTensor<double> Convolve2DUnrolled(const math::ConstChannelColumnRowTensorReference<double>& input, const math::ConstChannelColumnRowTensorReference<double>& filters, int numFilters);
}
}
