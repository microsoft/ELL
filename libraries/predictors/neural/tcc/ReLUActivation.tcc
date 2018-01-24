////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.tcc (neural)
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
    ElementType ReLUActivation<ElementType>::Apply(const ElementType input) const
    {
        return ((input > 0) ? input : 0);
    }

    template <typename ElementType>
    ElementType ReLUActivation<ElementType>::operator()(const ElementType input) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    ElementType ReLUActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet& /*index*/) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    void ReLUActivation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
    {
        input.Transform([this](ElementType value){ return Apply(value); });
    }
}
}
}
