////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformedConstVectorReference.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace math
{
    /// <summary> A class that represents a transformed constant vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    class TransformedConstVectorReference
    {
    public:
        /// <summary> Constructs an instance of TransformedConstVectorReference. </summary>
        ///
        /// <param name="vector"> The vector. </param>
        /// <param name="transform"> The transformation. </param>
        TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

        /// <summary> Gets the scalar. </summary>
        ///
        /// <returns> The scalar. </returns>
        const TransformationType GetTransformation() { return _transformation; }

        /// <summary> Gets the vector reference. </summary>
        ///
        /// <returns> The vector reference. </returns>
        ConstVectorReference<ElementType, orientation> GetVector() const { return _vector; }

    private:
        ConstVectorReference<ElementType, orientation> _vector;
        TransformationType _transformation;
    };

    /// <summary> Helper function that constructs a TransformedConstVectorReference from a vector and a transformation. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The vector. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> A TransformedConstVectorReference. </returns>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

    /// <summary> Multiplication operator for scalar and vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference that results from the operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise square operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise square-root operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square-root operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector);

    /// <summary> Elementwise absolute value operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise absolute value operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector);
}
}

#include "../tcc/TransformedConstVectorReference.tcc"