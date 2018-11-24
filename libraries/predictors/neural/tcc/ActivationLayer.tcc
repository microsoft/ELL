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
        template <typename ElementType>
        ActivationLayer<ElementType>::ActivationLayer(const LayerParameters& layerParameters, const ActivationType& activation) :
            Layer<ElementType>(layerParameters),
            _activation(activation)
        {
            ValidateDimensions();
        }

        template <typename ElementType>
        ActivationLayer<ElementType>::ActivationLayer(const ActivationLayer& other) :
            Layer<ElementType>(other),
            _activation(other._activation)
        {
        }

        template <typename ElementType>
        void ActivationLayer<ElementType>::ValidateDimensions()
        {
            auto output = GetOutputMinusPadding();
            auto& input = _layerParameters.input;
            if (input.NumRows() > output.NumRows() || input.NumColumns() > output.NumColumns() || input.NumChannels() > output.NumChannels())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input tensor must not exceed output tensor (minus padding) dimensions for activation layer.");
            }
        }

        template <typename ElementType>
        void ActivationLayer<ElementType>::Compute()
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
                        output(i, j, k) = _activation.ApplyIndex(value, math::IntegerTriplet{ i, j, k });
                    }
                }
            }
        }

        template <typename ElementType>
        void ActivationLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);
            _activation.WriteToArchive(archiver);
        }

        template <typename ElementType>
        void ActivationLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            if (archiver.HasNextPropertyName("activation"))
            {
                _activation.ReadFromArchive(archiver);
            }
            if (!_activation.GetImpl())
            {
                _activation.LegacyReadFromArchive(archiver);
            }
        }
    } // namespace neural
} // namespace predictors
} // namespace ell
