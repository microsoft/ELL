////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParametricReLUActivation.tcc (neural)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ElementType>
    ElementType ParametricReLUActivation<ElementType>::Apply(const ElementType input, const math::Triplet& index) const
    {
        return (( input > 0) ? input : _alpha(index) * input);
    }
}
}
}