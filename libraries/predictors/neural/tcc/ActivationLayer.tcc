////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <algorithm>

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

        auto flattenedInput = input.ReferenceAsMatrix();
        auto flattenedOutput = output.ReferenceAsMatrix();

        for (size_t i = 0; i < flattenedInput.NumRows(); i++)
        {
            auto rowVector = flattenedInput.GetMajorVector(i);
            for (size_t j = 0; j < rowVector.Size(); j++)
            {
                ElementType value = flattenedInput(i, j);
                flattenedOutput(i, j) = _activation.Apply(value);
            }
        }
    }

}
}
}
