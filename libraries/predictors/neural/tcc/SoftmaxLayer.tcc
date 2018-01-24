////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// STL
#include <limits>

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    SoftmaxLayer<ElementType>::SoftmaxLayer(const LayerParameters& layerParameters) :
        Layer<ElementType>(layerParameters)
    {
        if (_layerParameters.input.Size() != GetOutputMinusPadding().Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, GetRuntimeTypeName() + ": Expected size of input and output tensor (minus padding) to match");
        }
    }

    template <typename ElementType>
    void SoftmaxLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        AssignValues(input, output);

        ElementType sum = 0;
        ElementType maxValue = std::numeric_limits<ElementType>::lowest();

        // Find the max
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    ElementType value = input(i, j, k);
                    maxValue = std::max(maxValue, value);
                }
            }
        }

        // Use the max to calculate the Euler value
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    ElementType value = input(i, j, k);
                    ElementType eulerVal = std::exp(value - maxValue);
                    output(i, j, k) = eulerVal;
                    sum += eulerVal;
                }
            }
        }

        // Divide the value by the sum. After this, the sum of all values will be 1.0
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    output(i, j, k) /= sum;
                }
            }
        }

    }

}
}
}
