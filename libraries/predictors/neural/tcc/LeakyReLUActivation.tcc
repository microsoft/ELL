////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LeakyReLUActivation.h"

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

}
}
}