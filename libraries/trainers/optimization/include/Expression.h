////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Expression.h (optimization)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// math
#include "Vector.h"

// stl
#include <type_traits>

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> Enumeration of supported operations. </summary>
    enum class Operation { sum, product };

    /// <summary> Base class for types that can be summed. </summary>
    struct Summable {};

    /// <summary> Concept that checks if a given type is summable. </summary>
    template <typename T>
    using IsSummable = typename std::enable_if_t<std::is_base_of<Summable, T>::value, bool>;

    /// <summary> Represents a binary expression. </summary>
    template <Operation operation, typename LeftType, typename RightType>
    struct Expression : public Summable
    {
        /// <summary> Constructor. </summary>
        Expression(const LeftType& l, const RightType& r) : lhs(l), rhs(r) {}
        
        /// <summary> The left hand side term in the expression. </summary>
        LeftType lhs;

        /// <summary> The right hand side term in the expression. </summary>
        RightType rhs;
    };

    /// <summary> Helper function that makes it easily to create expressions. </summary>
    template <Operation operation, typename LeftType, typename RightType>
    Expression<operation, LeftType, RightType> MakeExpression(const LeftType& lhs, const RightType& rhs);

    /// <summary> Base class for types that can be multiplied by a scalar. </summary>
    struct Scalable {};

    /// <summary> Concept that checks if a given type is scalable. </summary>
    template <typename T>
    using IsScalable = typename std::enable_if_t<std::is_base_of<Scalable, T>::value, bool>;

    /// <summary> Convenient abbreviation of a vector-scalar product expression. </summary>
    template <typename ElementType>
    using ScaledColumnVectorExpression = Expression<Operation::product, math::ConstColumnVectorReference<ElementType>, double>;

    /// <summary> Multiplication operator for scalar and column vector. </summary>
    template <typename ElementType>
    ScaledColumnVectorExpression<ElementType> operator*(math::ConstColumnVectorReference<ElementType> vectorReference, double scalar);

    /// <summary> Convenient abbreviation of a vector-vector outer product expression. </summary>
    template <typename ElementType>
    using OuterProductExpression = Expression<Operation::product, math::ConstColumnVectorReference<ElementType>, math::ConstRowVectorReference<double>>;

    /// <summary> Multiplication (outer product) operator for column vector and row vector. </summary>
    template <typename ElementType>
    OuterProductExpression<ElementType> operator*(math::ConstColumnVectorReference<ElementType> columnVectorReference, math::ConstRowVectorReference<double> rowVectorReference);

    /// <summary> Convenient abbreviation of a multiplication expression for a Scalable type and a scalar. </summary>
    template <typename ScalableType>
    using ScaledExpression = Expression<Operation::product, std::reference_wrapper<const ScalableType>, double>;

    /// <summary> Multiplication operator for a Scalable type and a scalar. </summary>
    template <typename T, IsScalable<T> Concept = true>
    ScaledExpression<T> operator*(const T& scalable, double scalar);

    /// <summary> Convenient abbreviation of a sum expression. </summary>
    template <typename SummableType1, typename SummableType2>
    using SumExpression = Expression<Operation::sum, SummableType1, SummableType2>;

    /// <summary> Addition operator for two summable types. </summary>
    template <typename T1, typename T2, IsSummable<T1> Concept1 = true, IsSummable<T2> Concept2 = true>
    SumExpression<T1, T2> operator+(T1 summable1, T2 summable2);
}
}
}

#include "../tcc/Expression.tcc"