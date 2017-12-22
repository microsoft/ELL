////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Transformations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace math
{
    /// <summary> Transformations that apply to individual elements in a vector, matrix, or tensor. </summary>
    template<typename ElementType>
    using Transformation = ElementType(*)(ElementType);

    /// <summary> The square root transformation. </summary>
    template<typename ElementType>
    constexpr auto SquareRootTransformation = static_cast<Transformation<ElementType>>(std::sqrt);

    /// <summary> The cubic root transformation. </summary>
    template<typename ElementType>
    constexpr auto CubicRootTransformation = static_cast<Transformation<ElementType>>(std::cbrt);

    /// <summary> The absolute value transformation. </summary>
    template<typename ElementType>
    constexpr auto AbsoluteValueTransformation = static_cast<Transformation<ElementType>>(std::abs);

    /// <summary> The floor transformation. </summary>
    template<typename ElementType>
    constexpr auto FloorTransformation = static_cast<Transformation<ElementType>>(std::floor);

    /// <summary> The ceiling transformation. </summary>
    template<typename ElementType>
    constexpr auto CeilingTransformation = static_cast<Transformation<ElementType>>(std::ceil);

    /// <summary> The round transformation. </summary>
    template<typename ElementType>
    constexpr auto RoundTransformation = static_cast<Transformation<ElementType>>(std::round);

    /// <summary> The exponent transformation. </summary>
    template<typename ElementType>
    constexpr auto ExponentTransformation = static_cast<Transformation<ElementType>>(std::exp);

    /// <summary> The base 2 exponent transformation. </summary>
    template<typename ElementType>
    constexpr auto Base2ExponentTransformation = static_cast<Transformation<ElementType>>(std::exp2);

    /// <summary> The natural log transformation. </summary>
    template<typename ElementType>
    constexpr auto NaturalLogTransformation = static_cast<Transformation<ElementType>>(std::log);

    /// <summary> The base 2 log transformation. </summary>
    template<typename ElementType>
    constexpr auto Base2LogTransformation = static_cast<Transformation<ElementType>>(std::log2);

    /// <summary> The base 10 log transformation. </summary>
    template<typename ElementType>
    constexpr auto Base10LogTransformation = static_cast<Transformation<ElementType>>(std::log10);

    /// <summary> The sine transformation. </summary>
    template<typename ElementType>
    constexpr auto SineTransformation = static_cast<Transformation<ElementType>>(std::sin);

    /// <summary> The cosine transformation. </summary>
    template<typename ElementType>
    constexpr auto CosineTransformation = static_cast<Transformation<ElementType>>(std::cos);

    /// <summary> The tangent transformation. </summary>
    template<typename ElementType>
    constexpr auto TangentTransformation = static_cast<Transformation<ElementType>>(std::tan);

    /// <summary> The arc sine transformation. </summary>
    template<typename ElementType>
    constexpr auto ArcSineTransformation = static_cast<Transformation<ElementType>>(std::asin);

    /// <summary> The arc cosine transformation. </summary>
    template<typename ElementType>
    constexpr auto ArcCosineTransformation = static_cast<Transformation<ElementType>>(std::acos);

    /// <summary> The arc tangent transformation. </summary>
    template<typename ElementType>
    constexpr auto ArcTangentTransformation = static_cast<Transformation<ElementType>>(std::atan);

    /// <summary> The hyperbolic sine transformation. </summary>
    template<typename ElementType>
    constexpr auto HyperbolicSineTransformation = static_cast<Transformation<ElementType>>(std::sinh);

    /// <summary> The hyperbolic cosine transformation. </summary>
    template<typename ElementType>
    constexpr auto HyperbolicCosineTransformation = static_cast<Transformation<ElementType>>(std::cosh);

    /// <summary> The hyperbolic tangent transformation. </summary>
    template<typename ElementType>
    constexpr auto HyperbolicTangentTransformation = static_cast<Transformation<ElementType>>(std::tanh);

    /// <summary> The inverse hyperbolic sine transformation. </summary>
    template<typename ElementType>
    constexpr auto InverseHyperbolicSineTransformation = static_cast<Transformation<ElementType>>(std::asinh);

    /// <summary> The inverse hyperbolic cosine transformation. </summary>
    template<typename ElementType>
    constexpr auto InverseHyperbolicCosineTransformation = static_cast<Transformation<ElementType>>(std::acosh);

    /// <summary> The inverse hyperbolic tangent transformation. </summary>
    template<typename ElementType>
    constexpr auto InverseHyperbolicTangentTransformation = static_cast<Transformation<ElementType>>(std::atanh);

    /// <summary> The error function transformation. </summary>
    template<typename ElementType>
    constexpr auto ErrorFunctionTransformation = static_cast<Transformation<ElementType>>(std::erf);

    // Implementation of the SquareTransformation
    template <typename ElementType, utilities::IsFundamental<ElementType> concept = true>
    ElementType SquareTransformationImplementation(ElementType x)
    {
        return x * x; 
    }

    /// <summary> The square transformation. </summary>
    template<typename ElementType>
    constexpr auto SquareTransformation = static_cast<Transformation<ElementType>>(SquareTransformationImplementation);
}
}