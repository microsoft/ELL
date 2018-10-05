////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringUtil.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    FullyConnectedLayer<ElementType>::FullyConnectedLayer(const LayerParameters& layerParameters, ConstMatrixReferenceType& weights) :
        Layer<ElementType>(layerParameters),
        _weights(weights.NumRows(), weights.NumColumns()),
        _shapedInput(layerParameters.input.Size()),
        _outputVector(GetOutputMinusPadding().Size())
    {
        _weights = weights;
        if (_weights.NumRows() != GetOutputMinusPadding().Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                ell::utilities::FormatString("FullyConnectedLayer weights has %d row, but expecting %d based on output size minus padding",
                    _weights.NumRows(), GetOutputMinusPadding().Size()));
        }
    }

    template <typename ElementType>
    FullyConnectedLayer<ElementType>::FullyConnectedLayer(const LayerParameters& layerParameters, ConstTensorReferenceType& weights) :
        Layer<ElementType>(layerParameters),
        _weights(GetOutputMinusPadding().Size(), layerParameters.input.Size(), weights.ToArray()),
        _shapedInput(layerParameters.input.Size()),
        _outputVector(GetOutputMinusPadding().Size())
    {
        if (weights.Size() != GetOutputMinusPadding().Size() * layerParameters.input.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                ell::utilities::FormatString("FullyConnectedLayer weights size %d does not match output size (minus padding) of %d times input size %d",
                    _weights.NumRows(), GetOutputMinusPadding().Size(), layerParameters.input.Size()));
        }
    }    

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        // Reshape the input into a vector
        size_t columnIndex = 0;
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    _shapedInput[columnIndex++] = input(i, j, k);
                }
            }
        }

        math::MultiplyScaleAddUpdate((ElementType)1.0f, _weights, _shapedInput, (ElementType)0.0f, _outputVector);

        // Reshape the output
        columnIndex = 0;
        for (size_t i = 0; i < output.NumRows(); i++)
        {
            for (size_t j = 0; j < output.NumColumns(); j++)
            {
                for (size_t k = 0; k < output.NumChannels(); k++)
                {
                    output(i, j, k) = _outputVector[columnIndex++];
                }
            }
        }
    }

    template <typename ElementType>
    const typename FullyConnectedLayer<ElementType>::MatrixType& FullyConnectedLayer<ElementType>::GetWeights() const
    {
        return _weights;
    }

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);

        math::MatrixArchiver::Write(_weights, "weights", archiver);
    }

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        math::MatrixArchiver::Read(_weights, "weights", archiver);
        _shapedInput.Resize(_layerParameters.input.Size());
        _outputVector.Resize(GetOutputMinusPadding().Size());
    }

}
}
}

