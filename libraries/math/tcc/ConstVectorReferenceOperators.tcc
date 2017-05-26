
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstVectorReferenceOperators.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

namespace ell
{
namespace math
{
    /// <summary> Contains template code dependent on the Operations library. </summary>

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm0() const
    {
        return Operations::Norm0(*this);
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm1() const
    {
        return Operations::Norm1(*this);
    }

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm2() const
    {
        return Operations::Norm2(*this);
    }
}
}
