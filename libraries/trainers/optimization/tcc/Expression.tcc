////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Expression.tcc (optimization)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <Operation operation, typename LeftType, typename RightType>
        Expression<operation, LeftType, RightType> MakeExpression(const LeftType& lhs, const RightType& rhs)
        {
            return Expression<operation, LeftType, RightType>(lhs, rhs);
        }

        template <typename ElementType>
        ScaledColumnVectorExpression<ElementType> operator*(math::ConstColumnVectorReference<ElementType> vectorReference, double scalar)
        {
            return MakeExpression<Operation::product>(vectorReference, scalar);
        }

        template <typename ElementType>
        OuterProductExpression<ElementType> operator*(math::ConstColumnVectorReference<ElementType> columnVectorReference, math::ConstRowVectorReference<double> rowVectorReference)
        {
            return MakeExpression<Operation::product>(columnVectorReference, rowVectorReference);
        }

        template <typename T, IsScalable<T> Concept>
        ScaledExpression<T> operator*(const T& scalable, double scalar)
        {
            return MakeExpression<Operation::product>(std::ref(scalable), scalar);
        }

        template <typename T1, typename T2, IsSummable<T1> Concept1, IsSummable<T2> Concept2>
        SumExpression<T1, T2> operator+(T1 summable1, T2 summable2)
        {
            return MakeExpression<Operation::sum>(summable1, summable2);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell
