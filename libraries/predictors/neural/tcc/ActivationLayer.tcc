////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType, template <typename> class ActivationFunctionType>
    ActivationLayer<ElementType, ActivationFunctionType>::ActivationLayer(const LayerParameters& layerParameters) :
        Layer<ElementType>(layerParameters)
    {
        ValidateDimensions();
    }

    template <typename ElementType, template <typename> class ActivationFunctionType>
    ActivationLayer<ElementType, ActivationFunctionType>::ActivationLayer(const LayerParameters& layerParameters, ActivationFunctionType<ElementType> activation) :
        Layer<ElementType>(layerParameters), _activation(std::move(activation))
    {
        ValidateDimensions();
    }

    template <typename ElementType, template <typename> class ActivationFunctionType>
    void ActivationLayer<ElementType, ActivationFunctionType>::ValidateDimensions()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;
        if (input.NumRows() > output.NumRows() || input.NumColumns() > output.NumColumns() || input.NumChannels() > output.NumChannels())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input tensor must not exceed output tensor (minus padding) dimensions for activation layer.");
        }
    }

    template <typename ElementType, template <typename> class ActivationFunctionType>
    void ActivationLayer<ElementType, ActivationFunctionType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto input = _layerParameters.input;

        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    ElementType value = input(i, j, k);
                    output(i, j, k) = _activation.Apply(value, math::IntegerTriplet{i, j, k});
                }
            }
        }
    }

    template <typename ElementType, template <typename> class ActivationFunctionType>
    void ActivationLayer<ElementType, ActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Layer<ElementType>::WriteToArchive(archiver);
        _activation.WriteToArchive(archiver);
    }

    template <typename ElementType, template <typename> class ActivationFunctionType>
    void ActivationLayer<ElementType, ActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Layer<ElementType>::ReadFromArchive(archiver);
        _activation.ReadFromArchive(archiver);
    }
}
}
}
