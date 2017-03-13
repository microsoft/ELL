////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionalLayer.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        ConvolutionalLayer::ConvolutionalLayer(ConvolutionalParameters convolutionalParameters, const std::vector<double>& weights, const std::vector<double>& bias)
            : _convolutionalParameters(convolutionalParameters), _fieldVolumeSize(convolutionalParameters.receptiveField * convolutionalParameters.receptiveField * convolutionalParameters.depth), _outputWidth(((convolutionalParameters.width - convolutionalParameters.receptiveField + 2 * convolutionalParameters.padding) / convolutionalParameters.stride) + 1), _outputHeight(((convolutionalParameters.height - convolutionalParameters.receptiveField + 2 * convolutionalParameters.padding) / convolutionalParameters.stride) + 1), _numNeuronsPerFilter(_outputWidth * _outputHeight), _weights(convolutionalParameters.numFilters, _fieldVolumeSize, weights), _bias(bias), _shapedInput(_fieldVolumeSize, _numNeuronsPerFilter), _output(convolutionalParameters.numFilters * _numNeuronsPerFilter), _outputMatrixReference(convolutionalParameters.numFilters, _numNeuronsPerFilter, _output.GetDataPointer())
        {
            if (weights.size() != (_fieldVolumeSize * convolutionalParameters.numFilters))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "weights dimension for a convolutional layer should be the size of the receptive field volume * number of filters");
            }
        }

        ConvolutionalLayer::ConvolutionalLayer(ConvolutionalParameters convolutionalParameters, std::vector<double>&& weights, std::vector<double>&& bias)
            : _convolutionalParameters(convolutionalParameters), _fieldVolumeSize(convolutionalParameters.receptiveField * convolutionalParameters.receptiveField * convolutionalParameters.depth), _outputWidth(((convolutionalParameters.width - convolutionalParameters.receptiveField + 2 * convolutionalParameters.padding) / convolutionalParameters.stride) + 1), _outputHeight(((convolutionalParameters.height - convolutionalParameters.receptiveField + 2 * convolutionalParameters.padding) / convolutionalParameters.stride) + 1), _numNeuronsPerFilter(_outputWidth * _outputHeight), _weights(convolutionalParameters.numFilters, _fieldVolumeSize, std::move(weights)), _bias(std::move(bias)), _shapedInput(_fieldVolumeSize, _numNeuronsPerFilter), _output(convolutionalParameters.numFilters * _numNeuronsPerFilter), _outputMatrixReference(convolutionalParameters.numFilters, _numNeuronsPerFilter, _output.GetDataPointer())
        {
            if ((_weights.NumRows() * _weights.NumColumns()) != (_fieldVolumeSize * convolutionalParameters.numFilters))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "weights dimension for a convolutional layer should be the size of the receptive field volume * number of filters");
            }
        }

        ILayer::LayerVector& ConvolutionalLayer::FeedForward(const ILayer::LayerVector& input)
        {
            // Re-shape input.
            ReceptiveFieldToColumns(input.GetDataPointer(), _convolutionalParameters, _shapedInput.GetDataPointer());

            // Multiply reshaped input and weights.
            math::Operations::Multiply(1.0, _weights, _shapedInput, 0.0, _outputMatrixReference);

            // Add bias. The same bias gets added for all values in the same slice.
            size_t offset = 0;
            for (size_t k = 0; k < _bias.Size(); k++)
            {
                for (size_t i = 0; i < _numNeuronsPerFilter; i++)
                {
                    _output[offset + i] += _bias[k];
                }
                offset += _numNeuronsPerFilter;
            }

            return _output;
        }

        size_t ConvolutionalLayer::NumInputs() const
        {
            return (_convolutionalParameters.width * _convolutionalParameters.height * _convolutionalParameters.depth);
        }

        size_t ConvolutionalLayer::NumOutputs() const
        {
            return _output.Size();
        }

        double ConvolutionalLayer::GetValueFromVolume(const double* inputVolume, const ConvolutionalParameters& convolutionalParameters, intptr_t valueRow, intptr_t valueCol, intptr_t valueDepth)
        {
            if (convolutionalParameters.padding > 0)
            {
                valueRow -= convolutionalParameters.padding;
                valueCol -= convolutionalParameters.padding;

                if (valueRow < 0 || valueCol < 0 || valueRow >= (intptr_t)convolutionalParameters.height || valueCol >= (intptr_t)convolutionalParameters.width) return 0;
            }
            size_t valueOffset = (valueDepth * (convolutionalParameters.width * convolutionalParameters.height)) + (valueRow * convolutionalParameters.width) + valueCol;
            return inputVolume[valueOffset];
        }

        void ConvolutionalLayer::ReceptiveFieldToColumns(double* inputVolume, const ConvolutionalParameters& convolutionalParameters, double* outputMatrix)
        {
            size_t outIndex = 0;

            for (size_t f = 0; f < _fieldVolumeSize; f++)
            {
                size_t fieldColumn = f % convolutionalParameters.receptiveField; // field column offset
                size_t fieldRow = (f / convolutionalParameters.receptiveField) % convolutionalParameters.receptiveField; // field row offset
                size_t fieldDepth = (f / convolutionalParameters.receptiveField) / convolutionalParameters.receptiveField; // field depth offset

                size_t rowOffset = 0;
                for (size_t h = 0; h < _outputHeight; h++)
                {
                    size_t colOffset = 0;
                    for (size_t w = 0; w < _outputWidth; w++)
                    {
                        size_t input_row = rowOffset + fieldRow;
                        size_t input_col = colOffset + fieldColumn;

                        outputMatrix[outIndex] = GetValueFromVolume(inputVolume, convolutionalParameters, input_row, input_col, fieldDepth); // TODO: Optimize this with additions not row, col, depth access
                        outIndex++;
                        colOffset += convolutionalParameters.stride;
                    }
                    rowOffset += convolutionalParameters.stride;
                }
            }
        }

        void ConvolutionalLayer::WriteToArchive(utilities::Archiver& archiver) const
        {
            archiver["convolutionalParameters.width"] << _convolutionalParameters.width;
            archiver["convolutionalParameters.height"] << _convolutionalParameters.height;
            archiver["convolutionalParameters.depth"] << _convolutionalParameters.depth;
            archiver["convolutionalParameters.receptiveField"] << _convolutionalParameters.receptiveField;
            archiver["convolutionalParameters.stride"] << _convolutionalParameters.stride;
            archiver["convolutionalParameters.padding"] << _convolutionalParameters.padding;
            archiver["convolutionalParameters.numFilters"] << _convolutionalParameters.numFilters;
            archiver["outputWidth"] << _outputWidth;
            archiver["outputHeight"] << _outputHeight;
            archiver["numNeuronsPerFilter"] << _numNeuronsPerFilter;

            archiver["weights_rows"] << _weights.NumRows();
            archiver["weights_columns"] << _weights.NumColumns();
            std::vector<double> temp;
            temp.assign(_weights.GetDataPointer(), _weights.GetDataPointer() + (size_t)(_weights.NumRows() * _weights.NumColumns()));
            archiver["weights_data"] << temp;

            archiver["shapedInput_rows"] << _shapedInput.NumRows();
            archiver["shapedInput_columns"] << _shapedInput.NumColumns();
            temp.assign(_shapedInput.GetDataPointer(), _shapedInput.GetDataPointer() + (size_t)(_shapedInput.NumRows() * _shapedInput.NumColumns()));
            archiver["shapedInput_data"] << temp;

            temp = _bias.ToArray();
            archiver["bias"] << temp;

            temp = _output.ToArray();
            archiver["output"] << temp;
        }

        void ConvolutionalLayer::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            archiver["convolutionalParameters.width"] >> _convolutionalParameters.width;
            archiver["convolutionalParameters.height"] >> _convolutionalParameters.height;
            archiver["convolutionalParameters.depth"] >> _convolutionalParameters.depth;
            archiver["convolutionalParameters.receptiveField"] >> _convolutionalParameters.receptiveField;
            archiver["convolutionalParameters.stride"] >> _convolutionalParameters.stride;
            archiver["convolutionalParameters.padding"] >> _convolutionalParameters.padding;
            archiver["convolutionalParameters.numFilters"] >> _convolutionalParameters.numFilters;
            archiver["outputWidth"] >> _outputWidth;
            archiver["outputHeight"] >> _outputHeight;
            archiver["numNeuronsPerFilter"] >> _numNeuronsPerFilter;

            size_t rows = 0;
            size_t columns = 0;
            std::vector<double> temp;
            archiver["weights_rows"] >> rows;
            archiver["weights_columns"] >> columns;
            archiver["weights_data"] >> temp;
            _weights = ILayer::LayerMatrix(rows, columns, temp);

            archiver["shapedInput_rows"] >> rows;
            archiver["shapedInput_columns"] >> columns;
            archiver["shapedInput_data"] >> temp;
            _shapedInput = InputMatrix(rows, columns, temp);

            archiver["bias"] >> temp;
            _bias = ILayer::LayerVector(std::move(temp));

            archiver["output"] >> temp;
            _output = ILayer::LayerVector(std::move(temp));
            LayerReferenceMatrix tempReferenceMatrix(_convolutionalParameters.numFilters, _numNeuronsPerFilter, _output.GetDataPointer());
            _outputMatrixReference.Swap(tempReferenceMatrix);
        }
    }
}
}
