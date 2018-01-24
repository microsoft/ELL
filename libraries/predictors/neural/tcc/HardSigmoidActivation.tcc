////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationFunction.tcc (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    ElementType HardSigmoidActivation<ElementType>::Apply(const ElementType input) const
    {
        ElementType output = (static_cast<ElementType>(0.2) * input) + static_cast<ElementType>(0.5);
        return output < static_cast<ElementType>(0) ? static_cast<ElementType>(0) : (output > static_cast<ElementType>(1) ? static_cast<ElementType>(1) : output);
    }

    template <typename ElementType>
    ElementType HardSigmoidActivation<ElementType>::operator()(const ElementType input) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    ElementType HardSigmoidActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet& /*index*/) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    void HardSigmoidActivation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
    {
        input.Transform([this](ElementType value){ return Apply(value); });
    }
}
}
}
