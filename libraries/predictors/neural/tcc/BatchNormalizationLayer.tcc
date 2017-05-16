////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BatchNormalizationLayer.h"

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    BatchNormalizationLayer<ElementType>::BatchNormalizationLayer(const LayerParameters& layerParameters, const VectorType& mean, const VectorType& variance) :
        Layer<ElementType>(layerParameters),
        _multiplicationValues(mean.Size()),
        _additionValues(variance.Size())
    {
        if (mean.Size() != variance.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Size of 'mean' and 'variance' must match");
        }
        if (_layerParameters.input.NumElements() != GetOutputMinusPadding().NumElements())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Expected size of input and output tensor (minus padding) to match");
        }
        if (mean.Size() != NumOutputChannels())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Number of 'mean' and 'variance' values must equal number of channels in output");
        }

        // Batch norm is: outputValue = (inputValue - mean) / (sqrt(variance) + _epsilon)
        // To turn this into one MultiplyAdd operation, we can rearrange it to:
        // outputValue = inputValue * (1/(sqrt(variance) + _epsilon)) + (-mean * 1/(sqrt(variance) + _epsilon)) 
        for (size_t i = 0; i < _additionValues.Size(); i++)
        {
            ElementType varianceFactor = (1 / (std::sqrt(variance[i]) + _epsilon));

            _multiplicationValues[i] = varianceFactor;
            _additionValues[i] = -mean[i] * varianceFactor;
        }
    }

    template <typename ElementType>
    void BatchNormalizationLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto input = _layerParameters.input;

        AssignValues(input, output);
        math::TensorOperations::MultiplyAdd<math::Dimension::channel>(_multiplicationValues, _additionValues, output);
    }

    template <typename ElementType>
    void BatchNormalizationLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        // TODO:
    }

    template <typename ElementType>
    void BatchNormalizationLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        // TODO:
    }

}
}
}

