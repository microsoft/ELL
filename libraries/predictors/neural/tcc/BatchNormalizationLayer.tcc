////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    BatchNormalizationLayer<ElementType>::BatchNormalizationLayer(const LayerParameters& layerParameters, const VectorType& mean, const VectorType& variance, ElementType epsilon, EpsilonSummand epsilonSummand)
        : Layer<ElementType>(layerParameters), _multiplicationValues(mean.Size()), _additionValues(variance.Size()), _epsilon(epsilon), _epsilonSummand(epsilonSummand)
    {
        if (mean.Size() != variance.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Size of 'mean' and 'variance' must match");
        }
        if (_layerParameters.input.Size() != GetOutputMinusPadding().Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Expected size of input and output tensor (minus padding) to match");
        }
        if (mean.Size() != NumOutputChannels())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Number of 'mean' and 'variance' values must equal number of channels in output");
        }

        // Batch norm is: outputValue = (inputValue - mean) / (sqrt(variance) + _epsilon)
        // To turn this into one MultiplyAdd operation, we can rearrange it to:
        // EpsilonSummand::Variance:
        //   outputValue = inputValue * (1/(sqrt(variance + _epsilon))) + (-mean * 1/(sqrt(variance + _epsilon)))
        // EpsilonSummand::SqrtVariance:
        //   outputValue = inputValue * (1/(sqrt(variance) + _epsilon)) + (-mean * 1/(sqrt(variance) + _epsilon))
        for (size_t i = 0; i < _additionValues.Size(); i++)
        {
            ElementType varianceFactor = (_epsilonSummand == EpsilonSummand::Variance) ? (1 / (std::sqrt(variance[i] + _epsilon))) : (1 / (std::sqrt(variance[i]) + _epsilon));

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
        Layer<ElementType>::WriteToArchive(archiver);

        math::VectorArchiver::Write(_multiplicationValues, "multiplicationValues", archiver);
        math::VectorArchiver::Write(_additionValues, "additionValues", archiver);

        archiver["epsilon"] << _epsilon;
        archiver["epsilonSummand"] << static_cast<int>(_epsilonSummand);
    }

    template <typename ElementType>
    void BatchNormalizationLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);

        math::VectorArchiver::Read(_multiplicationValues, "multiplicationValues", archiver);
        math::VectorArchiver::Read(_additionValues, "additionValues", archiver);

        archiver["epsilon"] >> _epsilon;

        int value;
        archiver["epsilonSummand"] >> value;
        _epsilonSummand = static_cast<EpsilonSummand>(value);
    }
}
}
}
