////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    ConvolutionalLayer<ElementType>::ConvolutionalLayer(const LayerParameters& layerParameters, const ConvolutionalParameters& convolutionalParameters, TensorType weights) :
        Layer<ElementType>(layerParameters),
        _convolutionalParameters(convolutionalParameters),
        _weights(std::move(weights)),
        _shapedInput(convolutionalParameters.receptiveField * convolutionalParameters.receptiveField * _layerParameters.input.NumChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding()),
        _weightsMatrix(_layerParameters.outputShape[2], convolutionalParameters.receptiveField * convolutionalParameters.receptiveField * _layerParameters.input.NumChannels()),
        _outputMatrix(NumOutputChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding())
    {
        if(_weights.GetDataPointer() == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "weights tensor has null data field");
        }

        if (_weights.Size() != (_output.NumChannels() * _layerParameters.input.NumChannels() * convolutionalParameters.receptiveField * convolutionalParameters.receptiveField))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "weights dimensions for a convolutional layer should be the size of the receptive field volume * number of filters");
        }

        if (_convolutionalParameters.method == ConvolutionMethod::diagonal)
        {
            // Verify that we meet the criteria for doing Diagonal method. If not,
            // choose the normal method.
            if ((_convolutionalParameters.receptiveField % 2 == 0) || _convolutionalParameters.stride != 1)
            {
                _convolutionalParameters.method = ConvolutionMethod::columnwise;
            }
        }

        if (_convolutionalParameters.method == ConvolutionMethod::columnwise)
        {
            // Use the columnwise method
            // Reshape the weights
            auto flattened = _weights.ReferenceAsMatrix();
            for (size_t startRow = 0; startRow < flattened.NumRows() / convolutionalParameters.receptiveField; startRow++)
            {
                for (size_t row = 0; row < convolutionalParameters.receptiveField; row++)
                {
                    auto weightsVector = flattened.GetMajorVector(startRow * convolutionalParameters.receptiveField + row);
                    for (size_t i = 0; i < weightsVector.Size(); i++)
                    {
                        const size_t columnOffset = row * weightsVector.Size();
                        _weightsMatrix(startRow, columnOffset + i) = weightsVector[i];
                    }
                }
            }
        }
    }

    template <typename ElementType>
    void ConvolutionalLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        if (_convolutionalParameters.method == ConvolutionMethod::columnwise)
        {
            // Re-shape input.
            ReceptiveFieldToColumns(input, _shapedInput);

            // Multiply reshaped input and weights.
            math::Operations::Multiply(static_cast<ElementType>(1.0), _weightsMatrix, _shapedInput, static_cast<ElementType>(0.0), _outputMatrix);

            // Re-shape the output into the output tensor
            for (size_t i = 0; i < output.NumRows(); i++)
            {
                for (size_t j = 0; j < output.NumColumns(); j++)
                {
                    for (size_t k = 0; k < output.NumChannels(); k++)
                    {
                        size_t row = k;
                        size_t column = (i * output.NumColumns()) + j;
                        output(i, j, k) = _outputMatrix(row, column);
                    }
                }
            }
        }
        else
        {
            // Use the Diagonal method

            // Flatten the input
            auto inputMatrix = input.ReferenceAsMatrix();

            const size_t depth = input.NumChannels();
            const size_t kt = _convolutionalParameters.receptiveField * depth;
            const size_t paddingSize = _layerParameters.inputPaddingParameters.paddingSize;
            const size_t mPadding = paddingSize * depth;
            const size_t numConvolutions = (inputMatrix.NumColumns() - kt) / depth + 1;
            const size_t numFiltersAtAtime = _convolutionalParameters.numFiltersAtATime;
            const size_t numFilters = _layerParameters.outputShape[2];
            auto weightsMatrix = _weights.ReferenceAsMatrix().Transpose();

            for (size_t j = 0; j < numConvolutions; j++)
            {
                // Get the sub matrix for Vj
                auto Vj = inputMatrix.GetSubMatrix(0, j * depth, inputMatrix.NumRows(), kt);

                for (size_t filterStart = 0; filterStart < numFilters; filterStart += numFiltersAtAtime)
                {
                    size_t numFiltersToUse = std::min(numFiltersAtAtime, numFilters - filterStart);

                    auto Wl = weightsMatrix.GetSubMatrix(0, filterStart * _convolutionalParameters.receptiveField, weightsMatrix.NumRows(), numFiltersToUse * _convolutionalParameters.receptiveField);

                    MatrixType A(Vj.NumRows(), _convolutionalParameters.receptiveField * numFiltersToUse);

                    math::Operations::Multiply(static_cast<ElementType>(1.0), Vj, Wl, static_cast<ElementType>(0.0), A);

                    for (size_t l = 0; l < numFiltersToUse; l++)
                    {
                        for (size_t row = 0; row < (A.NumRows() - 2 * paddingSize); row++)
                        {
                            ElementType sum = 0.0;
                            for (size_t diagonal = 0; diagonal < _convolutionalParameters.receptiveField; diagonal++)
                            {
                                sum += A(row + diagonal, l * _convolutionalParameters.receptiveField + diagonal);
                            }
                            output(row, j, filterStart + l) = sum;
                        }
                    }
                }
            }
        }
    }

    template <typename ElementType>
    void ConvolutionalLayer<ElementType>::ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput)
    {
        size_t fieldVolumeSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels();
        size_t outIndex = 0;
        size_t convolutionalHeight = NumOutputRowsMinusPadding();
        size_t convolutionalWidth = NumOutputColumnsMinusPadding();

        for (size_t f = 0; f < fieldVolumeSize; f++)
        {
            size_t fieldDepth = f % _layerParameters.input.NumChannels();
            size_t fieldColumn = (f / _layerParameters.input.NumChannels()) % _convolutionalParameters.receptiveField;
            size_t fieldRow = (f / _layerParameters.input.NumChannels()) / _convolutionalParameters.receptiveField;

            size_t rowOffset = 0;
            for (size_t h = 0; h < convolutionalHeight; h++)
            {
                size_t colOffset = 0;
                for (size_t w = 0; w < convolutionalWidth; w++)
                {
                    size_t input_row = rowOffset + fieldRow;
                    size_t input_col = colOffset + fieldColumn;

                    shapedInput(f, h * convolutionalWidth + w) = input(input_row, input_col, fieldDepth);
                    colOffset += _convolutionalParameters.stride;
                }
                rowOffset += _convolutionalParameters.stride;
            }
        }
    }

    template <typename ElementType>
    void ConvolutionalLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);

        archiver["receptiveField"] << _convolutionalParameters.receptiveField;
        archiver["stride"] << _convolutionalParameters.stride;
        archiver["method"] << static_cast<int>(_convolutionalParameters.receptiveField);
        archiver["numFiltersAtATime"] << static_cast<int>(_convolutionalParameters.numFiltersAtATime);
        
        math::MatrixArchiver::Write(_shapedInput, "shapedInput", archiver);
        math::MatrixArchiver::Write(_weightsMatrix, "weightsMatrix", archiver);
        math::MatrixArchiver::Write(_outputMatrix, "outputMatrix", archiver);
    }

    template <typename ElementType>
    void ConvolutionalLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        archiver["receptiveField"] >> _convolutionalParameters.receptiveField;
        archiver["stride"] >> _convolutionalParameters.stride;
        archiver["method"] >> static_cast<int>(_convolutionalParameters.receptiveField);
        archiver["numFiltersAtATime"] >> static_cast<int>(_convolutionalParameters.numFiltersAtATime);

        math::MatrixArchiver::Read(_shapedInput, "shapedInput", archiver);
        math::MatrixArchiver::Read(_weightsMatrix, "weightsMatrix", archiver);
        math::MatrixArchiver::Read(_outputMatrix, "outputMatrix", archiver);
    }

}
}
}
