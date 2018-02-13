////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    namespace neural
    {

        template <typename ElementType>
        ConvolutionalLayer<ElementType>::ConvolutionalLayer(const LayerParameters& layerParameters, const ConvolutionalParameters& convolutionalParameters, TensorType weights)
            : Layer<ElementType>(layerParameters), _convolutionalParameters(convolutionalParameters), _weights(std::move(weights)), _shapedInput{ _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding() }, _weightsMatrix(_layerParameters.outputShape.NumChannels(), _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels()), _outputMatrix{ NumOutputChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding() }
        {
            if (_weights.GetDataPointer() == nullptr)
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
                    _convolutionalParameters.method = ConvolutionMethod::unrolled;
                }
            }

            ComputeWeightsMatrix();
        }

        template <typename ElementType>
        void ConvolutionalLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto& input = _layerParameters.input;

            switch (_convolutionalParameters.method)
            {
            case ConvolutionMethod::simple:
            {
                const int filterRows = static_cast<int>(_convolutionalParameters.receptiveField);
                const int filterColumns = filterRows;
                const int outputRows = static_cast<int>(output.NumRows());
                const int outputColumns = static_cast<int>(output.NumColumns());
                const int numFilters = static_cast<int>(output.NumChannels());
                for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
                {
                    const auto filterOffset = filterIndex * filterRows;
                    for (int i = 0; i < outputRows; ++i)
                    {
                        for (int j = 0; j < outputColumns; ++j)
                        {
                            ElementType accum = 0;
                            for (int filterRowIndex = 0; filterRowIndex < filterRows; ++filterRowIndex)
                            {
                                for (int filterColumnIndex = 0; filterColumnIndex < filterColumns; ++filterColumnIndex)
                                {
                                    auto signalVector = input.template GetSlice<math::Dimension::channel>(i + filterRowIndex, j + filterColumnIndex);
                                    auto filterVector = _weights.template GetSlice<math::Dimension::channel>(filterOffset + filterRowIndex, filterColumnIndex);
                                    accum += math::Dot(signalVector, filterVector);
                                }
                            }
                            output(i, j, filterIndex) = accum;
                        }
                    }
                }
            }
            break;
            case ConvolutionMethod::unrolled:
            {
                // Re-shape input.
                ReceptiveFieldToColumns(input, _shapedInput);

                // Multiply reshaped input and weights.
                math::MultiplyScaleAddUpdate(static_cast<ElementType>(1.0), _weightsMatrix, _shapedInput, static_cast<ElementType>(0.0), _outputMatrix);

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
            break;
            case ConvolutionMethod::diagonal:
            {
                // Use the Diagonal method

                // Flatten the input
                auto inputMatrix = input.ReferenceAsMatrix();

                const size_t depth = input.NumChannels();
                const size_t kt = _convolutionalParameters.receptiveField * depth;
                const size_t paddingSize = _layerParameters.inputPaddingParameters.paddingSize;
                const size_t numConvolutions = (inputMatrix.NumColumns() - kt) / depth + 1;
                const size_t numFiltersAtAtime = _convolutionalParameters.numFiltersAtATime;
                const size_t numFilters = _layerParameters.outputShape.NumChannels();
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

                        math::MultiplyScaleAddUpdate(static_cast<ElementType>(1.0), Vj, Wl, static_cast<ElementType>(0.0), A);

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
            break;
            }
        }

        template <typename ElementType>
        void ConvolutionalLayer<ElementType>::ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput)
        {
            size_t fieldVolumeSize = _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels();
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
            archiver["method"] << static_cast<int>(_convolutionalParameters.method);
            archiver["numFiltersAtATime"] << static_cast<int>(_convolutionalParameters.numFiltersAtATime);

            math::TensorArchiver::Write(_weights, "weights", archiver);
        }

        template <typename ElementType>
        void ConvolutionalLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            archiver["receptiveField"] >> _convolutionalParameters.receptiveField;
            archiver["stride"] >> _convolutionalParameters.stride;
            int method;
            archiver["method"] >> method;
            _convolutionalParameters.method = static_cast<ConvolutionMethod>(method);
            int numFilters;
            archiver["numFiltersAtATime"] >> numFilters;
            _convolutionalParameters.numFiltersAtATime = static_cast<size_t>(numFilters);

            math::TensorArchiver::Read(_weights, "weights", archiver);
            ComputeWeightsMatrix();
            InitializeIOMatrices();
        }

        template <typename ElementType>
        void ConvolutionalLayer<ElementType>::ComputeWeightsMatrix()
        {
            if (_convolutionalParameters.method == ConvolutionMethod::unrolled)
            {
                _weightsMatrix = { _layerParameters.outputShape.NumChannels(), _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels() };
                // Use the unrolled method
                // Reshape the weights
                auto flattened = _weights.ReferenceAsMatrix();
                for (size_t startRow = 0; startRow < flattened.NumRows() / _convolutionalParameters.receptiveField; startRow++)
                {
                    for (size_t row = 0; row < _convolutionalParameters.receptiveField; row++)
                    {
                        auto weightsVector = flattened.GetMajorVector(startRow * _convolutionalParameters.receptiveField + row);
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
        void ConvolutionalLayer<ElementType>::InitializeIOMatrices()
        {
            _shapedInput = { _convolutionalParameters.receptiveField * _convolutionalParameters.receptiveField * _layerParameters.input.NumChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding() };
            _outputMatrix = { NumOutputChannels(), NumOutputRowsMinusPadding() * NumOutputColumnsMinusPadding() };
        }
    }
}
}
