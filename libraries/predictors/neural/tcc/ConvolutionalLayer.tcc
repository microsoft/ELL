////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "DiagonalConvolution.h"
#include "SimpleConvolution.h"
#include "UnrolledConvolution.h"
#include "WinogradConvolution.h"

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

            _isDepthwiseSeparable = (_weights.NumChannels() == 1) && (_layerParameters.input.NumChannels() > 1);
            if (_isDepthwiseSeparable && (_output.NumChannels() != _layerParameters.input.NumChannels()))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input and output channel sizes must match for a depthwise-separable convolutional layer");
            }
            else if (!_isDepthwiseSeparable && (_weights.Size() != (_output.NumChannels() * _layerParameters.input.NumChannels() * convolutionalParameters.receptiveField * convolutionalParameters.receptiveField)))
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
            if (_convolutionalParameters.method == ConvolutionMethod::winograd)
            {
                // Verify that we meet the criteria for doing Winograd method. If not,
                // choose the normal method.
                if (_convolutionalParameters.stride != 1 || _convolutionalParameters.receptiveField != 3)
                {
                    _convolutionalParameters.method = ConvolutionMethod::unrolled;
                }
            }
            if (_isDepthwiseSeparable)
            {
                // Verify we can use a workable method for depthwise separable convolutions.
                if ((_convolutionalParameters.method != ConvolutionMethod::unrolled) && (_convolutionalParameters.method != ConvolutionMethod::simple) && (_convolutionalParameters.method != ConvolutionMethod::winograd))
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
            auto stride = static_cast<int>(_convolutionalParameters.stride);

            if (!_isDepthwiseSeparable)
            {
                switch (_convolutionalParameters.method)
                {
                case ConvolutionMethod::simple:
                {
                    const int numFilters = static_cast<int>(output.NumChannels());
                    dsp::Convolve2DSimple(input, _weights, numFilters, stride, output);
                }
                break;
                case ConvolutionMethod::unrolled:
                {
                    const int numFilters = static_cast<int>(output.NumChannels());
                    auto result = dsp::Convolve2DUnrolled(input, _weights, numFilters, stride);
                    output.CopyFrom(result);
                }
                break;
                case ConvolutionMethod::winograd:
                {
                    assert(stride == 1);
                    const int numFilters = static_cast<int>(output.NumChannels());
                    auto result = dsp::Convolve2DWinograd(input, _weights, numFilters);
                    output.CopyFrom(result);
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

                default:
                    throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Convolution method not supported");
                }
            }
            else // if _isDepthwiseSeparable
            {
                const int numFilters = 1;
                const size_t numInputRows = input.NumRows();
                const size_t numInputColumns = input.NumColumns();
                const size_t numOutputRows = output.NumRows();
                const size_t numOutputColumns = output.NumColumns();
                const size_t filterRows = _convolutionalParameters.receptiveField;

                for (size_t channel = 0; channel < output.NumChannels(); ++channel)
                {
                    using TensorType = typename Layer<ElementType>::TensorType;
                    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;

                    TensorType weights(_weights.GetSubTensor(filterRows * channel, 0, 0, filterRows, filterRows, 1));
                    const auto& inputChannelTensor = input.GetSubTensor(0, 0, channel, numInputRows, numInputColumns, 1);
                    TensorReferenceType outputChannelTensor = output.GetSubTensor(0, 0, channel, numOutputRows, numOutputColumns, 1);

                    switch (_convolutionalParameters.method)
                    {
                    case ConvolutionMethod::simple:
                    {
                        auto result = dsp::Convolve2DSimpleDepthwiseSeparable(inputChannelTensor, weights, numFilters, stride);
                        outputChannelTensor.CopyFrom(result);
                    }
                    break;
                    case ConvolutionMethod::unrolled:
                    {
                        auto result = dsp::Convolve2DUnrolled(inputChannelTensor, weights, numFilters, stride);
                        outputChannelTensor.CopyFrom(result);
                    }
                    break;
                    case ConvolutionMethod::winograd:
                    {
                        auto result = dsp::Convolve2DWinogradDepthwiseSeparable(inputChannelTensor, weights, numFilters, stride);
                        outputChannelTensor.CopyFrom(result);
                    }
                    break;
                    default:
                        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Convolution method not supported for depthwise separable convolution");
                    }
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
            _isDepthwiseSeparable = (_weights.NumChannels() == 1);
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
