////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SigmoidActivation.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType SigmoidActivation<ElementType>::Apply(const ElementType input) const
        {
            ElementType output;
            if (input >= 0.0)
            {
                double exp_value = std::exp(-input);
                output = static_cast<ElementType>(1.0 / (1.0 + exp_value));
            }
            else
            {
                double exp_value = std::exp(input);
                output = static_cast<ElementType>(exp_value / (1.0 + exp_value));
            }
            return output;
        }

        template <typename ElementType>
        ElementType SigmoidActivation<ElementType>::operator()(const ElementType input) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        ElementType SigmoidActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet& /*index*/) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        void SigmoidActivation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
        {
            input.Transform([this](ElementType value){ return Apply(value); });
        }
    }
}
}
