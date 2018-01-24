////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TanhActivation.tcc (neural)
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
    ElementType TanhActivation<ElementType>::Apply(const ElementType input) const
    {
        return std::tanh(input);
    }

    template <typename ElementType>
    ElementType TanhActivation<ElementType>::operator()(const ElementType input) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    ElementType TanhActivation<ElementType>::Apply(const ElementType input, const math::IntegerTriplet&) const
    {
        return Apply(input);
    }

    template <typename ElementType>
    void TanhActivation<ElementType>::Apply(math::ColumnVectorReference<ElementType>& input) const
    {
        input.Transform([this](ElementType value){ return Apply(value); });
    }
}
}
}
