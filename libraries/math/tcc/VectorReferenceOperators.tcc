
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorReferenceOperators.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

namespace ell
{
namespace math
{
    /// <summary> Contains template code dependent on the Operations library. </summary>

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::Set(ConstVectorReference<ElementType, orientation> other)
    {
        Operations::Copy(other, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator+=(ConstVectorReference<ElementType, orientation> other)
    {
        Operations::Add(static_cast<ElementType>(1.0), other, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator=(ConstVectorReference<ElementType, orientation> other)
    {
        Operations::Copy(other, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator-=(ConstVectorReference<ElementType, orientation> other)
    {
        Operations::Add(static_cast<ElementType>(-1.0), other, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator+=(ElementType value)
    {
        Operations::Add(value, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator-=(ElementType value)
    {
        Operations::Add(-value, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator*=(ElementType value)
    {
        Operations::Multiply(value, *this);
    }

    template <typename ElementType, VectorOrientation orientation>
    void VectorReference<ElementType, orientation>::operator/=(ElementType value)
    {
        if (value == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero");
        }
        Operations::Multiply(static_cast<ElementType>(1.0) / value, *this);
    }
}
}
