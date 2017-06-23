////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftMaxActivation.tcc (neural)
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
    void SoftMaxActivation<ElementType>::Apply(const VectorType& input, VectorType& output) const
    {
        if (input.Size() > output.Size())
        {
            throw std::invalid_argument("Expected output vector to have at least as many elements as the input vector");
        }

        double sum = 0;
        double maxVal = -std::numeric_limits<double>::max();
        for (size_t i = 0; i < input.Size(); i++)
        {
            maxVal = std::max(maxVal, input[i]);
        }
        for (size_t i = 0; i < input.Size(); i++)
        {
            double eulerVal = std::exp(input[i] - maxVal);
            output[i] = eulerVal;
            sum += eulerVal;
        }
        for (size_t i = 0; i < input.Size(); i++)
        {
            output[i] /= sum;
        }
    }

    template <typename ElementType>
    void SoftMaxActivation<ElementType>::Apply(TensorReferenceType& input, WriteableTensorReferenceType& output) const
    {
        if (input.NumElements() > output.NumElements())
        {
            throw std::invalid_argument("Expected output tensor to have at least as many elements as the input tensor");
        }

        ElementType sum = 0;
        ElementType maxVal = -std::numeric_limits<ElementType>::max();

        // Find the max
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    ElementType value = input(i, j, k);
                    maxVal = std::max(maxVal, value);
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
                    ElementType eulerVal = std::exp(value - maxVal);
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
