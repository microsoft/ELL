////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.tcc (neural)
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
    ElementType LeakyReLUActivation<ElementType>::Apply(const ElementType input) const
    {
        return (( input > 0) ? input : _leakyFactor * input);
    }

    template <typename ElementType>
    ElementType LeakyReLUActivation<ElementType>::operator()(const ElementType input) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    ElementType LeakyReLUActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet& /*index*/) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    void LeakyReLUActivation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
    {
        input.Transform([this](ElementType value){ return Apply(value); });
    }
}
}
}
